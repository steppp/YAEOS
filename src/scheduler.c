#include <scheduler.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>

pcb_t *readyQueue;
pcb_t *runningPcb;

unsigned readyPcbs;
unsigned softBlockedPcbs;
unsigned activePcbs;

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
        pcb_t *tmp = NULL;
        if (runningPcb != NULL)
        {
            state_t curstate;
            STST(&curstate);
            // save the processor state in the process' pcb
            runningPcb->p_s = curstate;
            // restoring its old priority
            runningPcb->p_priority = runningPcb->old_priority;
            tmp = runningPcb;
        }
        runningPcb = removeProcQ(&readyQueue);
        if (tmp != NULL)
            insertProcQ(&readyQueue,tmp);
        LDST(&runningPcb->p_s); // load the new PCB
    }
}
