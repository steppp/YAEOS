#include <interrupts.h>
#include <arch.h>
#include <uARMconst.h>
#include <libuarm.h>
#include <scheduler.h>
#include <syscall.h>
#include <list.h>
#include <pcb.h>
#include <main.h>

// TODO: queste variabili sono presenti anche nel file main.h - devono essere dichiarate anche qui o è un errore?
int disks[DEVICES];
int tapes[DEVICES];
int networks[DEVICES];
int printers[DEVICES];
int terminals[DEVICES][2];

int pseudoClockTicks;    /* Number of times the pseudoclock caused an interrupt */
int agingTicks;  /* Number of times the aging caused an interrupt */

void interruptDispatcher()
{
    /*
        Understand from which line the interrupt came from
        if line == IL_TIMER
            call the timer handler routine
        else
            
     */
    if (CAUSE_IP_GET(getCAUSE(),INT_TIMER))
        handleTimer();
    else
    {
        int i;  /* Contains the interrupt line */
        int j;  /* Contains the interrupt device */
        for (i = INT_DISK; i <= INT_TERMINAL; i++)
            if (CAUSE_IP_GET(getCAUSE(),i))
            {
                int bitmap = CDEV_BITMAP_ADDR(i);
                for (j = 0; j < BYTELEN; j++)
                    if (bitmap & (1 << j))
                        break;
                break;
            }
        /*
           incomplete part ahead
         */
        switch (i)
        {
            case INT_DISK:
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
            pseudoClockTicks++;
            while (pseudoClockSem < 0)
                V(&pseudoClockSem);
            if (runningPcb == NULL)
                dispatch();
            break;
        case TIMESLICE:
            p = suspend();
            /* TODO Save the user, kernel and wall clock time in p */
            updateTimer(TOD);
            dispatch();
            break;
        case AGING:
            agingTicks++;
            forallProcQ(readyQueue,increasePriority,NULL);
            updateTimer(TOD);
            break;
    }
}

void updateTimer()
{
    unsigned int scale = *((unsigned int *) BUS_REG_TIME_SCALE);    /* Needed to conver CPU cycle in
                                                                     micro seconds */
    int pseudoDeadline, agingDeadline;
    pseudoDeadline = (clockStart + ((pseudoClockTicks + 1)*PSEUDOCLOCKPERIOD)*scale - getTODLO());
    /* Time remaining until the next pseudoClockTick, in numbers of CPU cycles */
    if (pseudoDeadline <= TIMESLICEPERIOD*scale)
    {
        lastTimerCause = PSEUDOCLOCK;   /* The next interrupt will be a pseudoclock interrupt */
        setTIMER(pseudoDeadline);
    }
    else
    {
        agingDeadline = (clockStart + ((agingTicks + 1)*AGINGPERIOD)*scale - getTODLO());
        if (agingDeadline <= TIMESLICEPERIOD*scale)
        {
            lastTimerCause = AGING;
            setTIMER(agingDeadline);
        }
        else
        {
            lastTimerCause = TIMESLICE;
            setTIMER(TIMESLICEPERIOD*scale);
        }
    }
}
