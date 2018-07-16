#include <scheduler.h>
#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <main.h>
#include <interrupts.h>
#include <const.h>

void dispatch()
{
    /*
        Check if there are processes in the ready queue.
            If there are, check if there is a running process.
                If there is one, load CPU state in a temp variable and save a temp pointer to it.
                Load the first process in the ready queue on the cpu, removing it
            Else
                do nothing
   */
    if (readyPcbs > 0)
    {
        pcb_t *tmp = NULL;  /* temp var to store the running pcb */
        if (runningPcb != NULL) /* TODO Never happens, probably gonna remove this */
            tmp = suspend();
        runningPcb = removeProcQ(&readyQueue);
        readyPcbs--;
        if (tmp != NULL)
            insertProcQ(&readyQueue,tmp);
        updateTimer();  /* Load the new timer */
        LDST(&runningPcb->p_s); /* load the new PCB */
    }
    else
    {
        int status = getSTATUS();
        setSTATUS(STATUS_ALL_INT_ENABLE(status));
        WAIT(); /* wait for an interrupt */
    }
}

pcb_t *suspend()
{
    if (runningPcb != NULL)
    {
        pcb_t *p;   /* holds the running pcb pointer for return*/
        // runningPcb->p_s = *oldarea; /* save the old processor state in the process' pcb */
        runningPcb->p_priority = runningPcb->old_priority; /* restoring its old priority */
        p = runningPcb;
        runningPcb = NULL;
        return p;
    }
    else
        return NULL;
}

void increasePriority(pcb_t *p, void *arg)
{
    for (pcb_t *i = p; i != NULL; i = i->p_next)
        if (p->p_priority < MAXPRIO)
            i->p_priority++;
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
