#include <scheduler.h>
#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <main.h>

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
        pcb_t *tmp = NULL;  /* temp var to store the running pcb*/
        if (runningPcb != NULL)
            tmp = suspend();
        runningPcb = removeProcQ(&readyQueue);
        readyPcbs--;
        if (tmp != NULL)
            insertProcQ(&readyQueue,tmp);
        LDST(&runningPcb->p_s); // load the new PCB
    }
}

pcb_t *suspend()
{
    if (runningPcb != NULL)
    {
        pcb_t *p;   /* holds the running pcb pointer for return*/
        runningPcb->p_s = *((state_t *) SYSBK_OLDAREA); /* save the old processor state in the process' pcb */
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
        i->p_priority++;
}
