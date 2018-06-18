#include <interrupts.h>
#include <arch.h>
#include <uARMconst.h>
#include <libuarm.h>
#include <scheduler.h>
#include <syscall.h>
#include <list.h>
#include <pcb.h>
#include <main.h>

devhdl_t *deviceHandlers[] = {handleDisk, handleTape, handleNetwork, handlePrinter, handleTerminal};
/* Contains the addresses of the device handling routines TODO move it to initialization */

void interruptDispatcher()
{
    /*
        Understand from which line the interrupt came from
        if line == IL_TIMER
            call the timer handler routine
        else
            calculate the device register address from line and cause
            call the specific device hanlder routine, giving the device register as an argument
     */
    if (CAUSE_IP_GET(getCAUSE(),INT_TIMER))
        handleTimer();
    else
    {
        for (int i = INT_DISK; i < INT_TERMINAL; i++)
            if (CAUSE_IP_GET(getCAUSE(),i))
            {
                int bitmap = CDEV_BITMAP_ADDR(i);
                int j;
                for (j = 0; j < BYTELEN; j++)
                    if (bitmap & (1 << j))
                        break;
                devreg_t *devreg = (devreg_t *) (DEV_REG_ADDR(i,j));
                deviceHandlers[i-3](devreg);
                break;
            }
    }
}

int handleTimer()
{
    /* 
       get Time of the day and save it in a variable
        case lastTimer of
            PSEUDOCLOCK
                unblock all processes on pseudoclockSem
                updateTimer(TOD)
            TIMESLICE
                suspend the current process
                save its user, kernel and wall clock time in the PCB
                updateTimer(TOD)
                dispatch()
            AGING
                increase the priority of all processes in the ready queue by one
                updateTimer()
     */
    cpu_t TOD = getTODLO();
    pcb_t *p;
    switch (lastTimerCause)
    {
        case PSEUDOCLOCK:
            while (pseudoClockSem < 0)
                V(&pseudoClockSem);
            updateTimer(TOD);
            break;
        case TIMESLICE:
            p = suspend();
            /* Save the user, kernel and wall clock time in p */
            updateTimer(TOD);
            dispatch();
            break;
        case AGING:
            forallProcQ(readyQueue,increasePriority,NULL);
            updateTimer(TOD);
            break;
    }
}

void updateTimer(cpu_t TOD)
{
    unsigned int scale = *((unsigned int *) BUS_REG_TIME_SCALE);    /* Needed to conver CPU cycle in
                                                                     micro seconds */
    cpu_t elapsedTime = (TOD - getTODLO()) / scale;    /* Approximately the time spent in the
                                                          interval time interrupt service routine */
    timeSliceTimer = MIN(0,timeSliceTimer - elapsedTime);
    agingTimer = MIN(0,timeSliceTimer - elapsedTime);
    pseudoClockTimer = MIN(0,timeSliceTimer - elapsedTime);
    if (pseudoClockTimer <= agingTimer && pseudoClockTimer <= timeSliceTimer)
    {
        lastTimerCause = PSEUDOCLOCK;
        setTIMER(pseudoClockTimer * scale);
        agingTimer -= pseudoClockTimer;
        timeSliceTimer -= pseudoClockTimer;
        pseudoClockTimer = PSEUDOCLOCKPERIOD - elapsedTime; /* need to be precise */
    }
    else if (timeSliceTimer <= pseudoClockTimer && timeSliceTimer <= agingTimer)
    {
        lastTimerCause = TIMESLICE;
        setTIMER(timeSliceTimer * scale);
        pseudoClockTimer -= timeSliceTimer;
        agingTimer -= timeSliceTimer;
        timeSliceTimer = TIMESLICEPERIOD;
    }
    else
    {
        lastTimerCause = AGING;
        setTIMER(agingTimer * scale);
        pseudoClockTimer -= agingTimer;
        timeSliceTimer -= agingTimer;
        agingTimer = AGINGPERIOD;
    }
}
