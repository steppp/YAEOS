#include <interrupts.h>
#include <arch.h>
#include <uARMconst.h>
#include <libuarm.h>
#include <scheduler.h>
#include <syscall.h>
#include <semaphore.h>
#include <list.h>
#include <pcb.h>
#include <main.h>

int normalDevices[N_INTERRUPT_LINES - 4][DEV_PER_INT];
int terminals[DEV_PER_INT][2];

int pseudoClockTicks;    /* Number of times the pseudoclock caused an interrupt */
int agingTicks;  /* Number of times the aging caused an interrupt */

void interruptHandler()
{
    /*
       Understand from which line the interrupt came from
       if line == IL_TIMER
       call the timer handler routine
       else
       find the appropriate device register
       find the appropriate device semaphore
       V on that semaphore
       Ack the interrupt
     */
    if (CAUSE_IP_GET(getCAUSE(),INT_TIMER))
        handleTimer();
    else
    {
        int i;  /* Contains the interrupt line */
        int j;  /* Contains the interrupt device */
        unsigned int cause = getCAUSE();    /* interrupt cause (CP15_Cause) */
        for (i = INT_DISK; i <= INT_TERMINAL; i++)
            if (CAUSE_IP_GET(cause,i))
            {
                int bitmap = CDEV_BITMAP_ADDR(i);
                for (j = 0; j < BYTELEN; j++)
                    if (bitmap & (1 << j))
                        break;
                break;
            }
        devreg_t *deviceRegister = (devreg_t *)DEV_REG_ADDR(i,j);
        pcb_t *p;   /* first blocked process on the semaphore */
        int which = -1; /* If the interrupt was from a terminal, this discriminates between
                           transimmsion and receipt */
        switch(i)
        {
            /*
               Single semaphore devices are handled in a uniform way
             */
            case INT_DISK:
            case INT_TAPE:
            case INT_UNUSED:    /* in uARMconst.h this is the definition of the Network interrupt line */
            case INT_PRINTER:
                p = headBlocked(&normalDevices[i-INT_LOWEST][j]);   /* INT_LOWEST is an offset of 3,
                                                                       to map each device >= 3 to
                                                                       the right semaphore */
                if (p == NULL)
                {
                    tprint("No device blocked on semaphore (MESSAGE BY KERNEL, NOT P2TEST)");
                    PANIC();
                }
                p->p_s.a1 = deviceRegister->dtp.status; /* Returning the status of the device */
                V(&normalDevices[i-INT_LOWEST][j]);
                deviceRegister->dtp.command = DEV_C_ACK; /* acknowledging the interrupt */
                break;
            case INT_TERMINAL:
                /* I must determine which of the two subdevices generated the interrupt */
                if (deviceRegister->term.transm_status == DEV_TTRS_S_CHARTRSM) /* Successful
                                                                                 transmission */
                    which = TRANSM;
                else if (deviceRegister->term.recv_status == DEV_TRCV_S_CHARRECV) /* Successful
                                                                                    receipt */
                    which = RECV;
                else
                {
                    tprint("Failed both transmission and receipt (MESSAGE BY KERNEL, NOT P2TEST)");
                    PANIC();
                }
                p = headBlocked(&terminals[j][which]);
                if (p == NULL)
                {
                    tprint("No device blocked on semaphore (MESSAGE BY KERNEL, NOT P2TEST)");
                    PANIC();
                }
                if (which == TRANSM) /* Returning the status of the device */
                    p->p_s.a1 = deviceRegister->term.transm_status; 
                else if (which == RECV)
                    p->p_s.a1 = deviceRegister->term.recv_status;
                V(&terminals[j][which]);
                if (which == TRANSM) /* Returning the status of the device */
                    deviceRegister->term.transm_command = DEV_C_ACK; /* acknowledging the interrupt */
                else if (which == RECV)
                    deviceRegister->term.recv_command = DEV_C_ACK; /* acknowledging the interrupt */
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
