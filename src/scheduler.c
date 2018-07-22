#include <scheduler.h>
#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <main.h>
#include <interrupts.h>
#include <const.h>

void dispatch(state_t *to_save)
{
    /*
        Check if there are processes in the ready queue.
            If there are, check if there is a running process.
                If there is one, load CPU state in a temp variable and save a temp pointer to it.
                Load the first process in the ready queue on the cpu, removing it
            Else
                check if there could be a deadlock
                if so, halt
                else, wait for an interrupt
   */
    if (readyPcbs > 0)
    {
        pcb_t *p;
        p = removeProcQ(&readyQueue);
        readyPcbs--;
        if (runningPcb != NULL)
            insertInReady(runningPcb,to_save);
        runningPcb = p;
        updateTimer();  /* Load the new timer */
        freezeLastTime(p); /* Freezing the lasttime in pcb for calculating next user time */
        LDST(&runningPcb->p_s); /* load the new PCB */
    }
    else
    {
        int status = getSTATUS();
        updateTimer();
        setSTATUS(STATUS_ALL_INT_ENABLE(status));
        WAIT(); /* wait for an interrupt */
    }
}

pcb_t *suspend()
{
    if (runningPcb != NULL)
    {
        pcb_t *p;   /* holds the running pcb pointer for return*/
        runningPcb->p_priority = runningPcb->old_priority; /* restoring its old priority */
        p = runningPcb;
        runningPcb = NULL;
        return p;
    }
    else
        return NULL;
}

void increasePriority(pcb_t *q, void *arg)
{
    if (q != NULL)
    {
        for (pcb_t *i = q; i != NULL; i = i->p_next)
            if (i->p_priority < MAXPRIO)
            {
                i->p_priority++;
                if (runningPcb != NULL && i->p_priority > runningPcb->p_priority)
                    insertInReady(runningPcb,(state_t *)INT_OLDAREA);
            }
    }
}

/* Gets called when there are no more ready processes and handles all possibilities */
void noMoreReadyPcbs(){

    /* If there are no more processed in the ready queue and no active ones the system has done its job and needs to shut down */
    if (readyPcbs==0 && activePcbs==0){ 
    tprint("Shutting down. Goodnight sweet prince (MESSAGE BY KERNEL, NOT P2TEST)");    
    HALT();
    }
    /* If there are no more processed in the ready queue but some processes are soft-blocked then the system needs to be put in a Wait state to wait for an interrupt */
    else if(readyPcbs==0 && softBlockedPcbs!=0){
    tprint("Twiddling thumbs mode initialized (MESSAGE BY KERNEL, NOT P2TEST)");
    WAIT();
    }
    /* If there are no more processed in the ready queue and no processes are soft-blocked then the system is probably in deadlock */
    else if (readyPcbs==0 && softBlockedPcbs==0){
    tprint("Deadlock detected, shutting down (MESSAGE BY KERNEL, NOT P2TEST)");    
    PANIC();
    }
}

void restoreRunningProcess(state_t *oldarea)
{
    if (oldarea == (state_t *)INT_OLDAREA)
        oldarea->pc -= 4; /* Restoring the right return address*/
    
    updateTimer();
    freezeLastTime(runningPcb); /* Freezing the lasttime in pcb for calculating next user time */
    LDST(oldarea);
}

int insertInReady(pcb_t *p, state_t *to_save)
{
    if (p != NULL)
    {
        int flag; /* 0 if we are inserting a different process from the running one in the ready
                       queue, 1 if we are re-inserting the runningProcess in the ready Queue */
        flag = (p == runningPcb);
        insertProcQ(&readyQueue,p);
        readyPcbs++;
        if (runningPcb != NULL && (p->p_priority > runningPcb->p_priority || flag)) 
            /*
                The second part of the or is used when, during the dipatch function, the running
                process needs to be stopped and saved
             */
        {
            pcb_t *q = suspend(); /* Points to the pcb of the process running before this function
                                     was called */ 
            if (to_save != NULL)
            {
                if (to_save == (state_t*) INT_OLDAREA)
                    to_save->pc -= 4; /* Restoring the pc to the right value */
                q->p_s = *to_save; /* Saving the process state in its PCB */
            }
            if (!flag)
            {
                insertProcQ(&readyQueue,q);
                readyPcbs++;
            }
        }
        return 0;
    }
    else
        return -1;
}

/* Facility for accounting new user time */
void userTimeAccounting(unsigned int TOD_Hi, unsigned int TOD_Low) {
    /* Now creating cpu_t time having High and Low part of the number */
    cpu_t newUserTime = TOD_Hi; /* Assigning the Hi part at the variable... */
    newUserTime <<= 32; /* ..shifting in the Hi part of the number... */
    newUserTime += TOD_Low; /* ...and sum the lower part */

    newUserTime -= runningPcb->lasttime; /* Substract the time calculated to the last time marked */
    runningPcb->usertime += newUserTime; /* Sum the slice of time to the total user time of process  */
}

/* Facility for accounting new user time */
void kernelTimeAccounting(unsigned int TOD_Hi, unsigned int TOD_Low, pcb_t* process) {
    cpu_t newKernelTime, nowTOD;

    /* If no process has ho account time, do nothing*/
    if (process == NULL) return;

    /* Now creating cpu_t time having High and Low part of the number */
    newKernelTime = TOD_Hi; /* Assigning the Hi part at the variable... */
    newKernelTime <<= 32; /* ..shifting in the Hi part of the number... */
    newKernelTime += TOD_Low; /* ...and sum the lower part */

    /* Doing the same stuff, but using current TOD */
    nowTOD = getTODHI();
    nowTOD <<= 32;
    nowTOD += getTODLO();

    nowTOD -= newKernelTime; /* Substract the TOD to the kernel time calculated  */
    runningPcb->kerneltime += nowTOD; /* Sum the slice of time to the total kernel time */
}

/* TOD of when a process becomes running */
void freezeLastTime(pcb_t *p) {
    /* Now creating cpu_t time having High and Low part of the number.
       Save it to pcb's lasttime. */
    p->lasttime = getTODHI(); /* Assigning the Hi part at the variable... */
    p->lasttime <<= 32; /* ..shifting in the Hi part of the number... */
    p->lasttime += getTODLO(); /* ...and sum the lower part */
}