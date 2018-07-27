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
     *   Check if there are processes in the ready queue.
     *      If there are
     *          remove the first one and put it in a variable
     *          check if there is a running process.
     *          If there is one
     *              insert it in the ready queue
     *          set the running process to the removed one
     *          update the interval Timer
     *          Save the time of loading
     *          Load the running process
     *      Else
     *          checks if there could be a deadlock or if its time for a shutdown
     *          if so, halts
     *          else, wait for an interrupt
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
        /* If there are no more processes in the ready queue and no active ones the system has done its job and needs to shut down */
        if (activePcbs == 0 && softBlockedPcbs == 0){ 
            tprint("Shutting down\n");    
            HALT();
        }
        /* If there are no more processes in the ready queue but some processes are soft-blocked then the system needs to be put in a Wait state to wait for an interrupt */
        else if((softBlockedPcbs!=0) || (pseudoClockSem < 0)){
            int status = getSTATUS();
            updateTimer();
            setSTATUS(STATUS_ALL_INT_ENABLE(status));
            WAIT();              
        }
        /* If there are no more processes in the ready queue and no processes are soft-blocked then the system is probably in deadlock */
        else if (activePcbs!=0){
            tprint("Deadlock detected, panicking (MESSAGE BY KERNEL, NOT P2TEST)");    
            PANIC();
        }
    }
}

pcb_t *suspend()
{
    /*
     *  If there is a running process, 
     *      save it into a variable
     *      restore its original priority
     *      set the running process to null
     *      return the process
     *  else
     *      return NULL      
     */
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
        if (q->p_priority < MAXPRIO)
            q->p_priority++;
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
             * The second part of the or is used when, during the dipatch function, the running
             *  process needs to be stopped and saved
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

    /* If no process has no account time, do nothing*/
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
    process->kerneltime += nowTOD; /* Sum the slice of time to the total kernel time */
}

/* TOD of when a process becomes running */
void freezeLastTime(pcb_t *p) {
    /* Now creating cpu_t time having High and Low part of the number.
       Save it to pcb's lasttime. */
    p->lasttime = getTODHI(); /* Assigning the Hi part at the variable... */
    p->lasttime <<= 32; /* ..shifting in the Hi part of the number... */
    p->lasttime += getTODLO(); /* ...and sum the lower part */
}

int passup(state_t *old_to_save)
{

    if ((runningPcb == NULL) || (old_to_save == NULL))
        return 0;
    else
    {
        state_t *new,*old;
        new = NULL;
        old = NULL;
        if (old_to_save == (state_t*) PGMTRAP_OLDAREA)
        {
            new = runningPcb->pgmtrap_new;
            old = runningPcb->pgmtrap_old;
        }
        else if (old_to_save == (state_t*) TLB_OLDAREA)
        {
            new = runningPcb->tlb_new;
            old = runningPcb->tlb_old;
        }
        else if (old_to_save == (state_t*) SYSBK_OLDAREA)
        {
            new = runningPcb->sysbk_new;
            old = runningPcb->sysbk_old;
        }
        if ((new == NULL) || (old == NULL))
            return 0;
        else
        {
            *old = *old_to_save;
            kernelTimeAccounting(((state_t *) TLB_OLDAREA) ->TOD_Hi, ((state_t *) TLB_OLDAREA)
                    ->TOD_Low, runningPcb); /* Calculating kernel times */
            updateTimer();
            freezeLastTime(runningPcb);
            LDST(new);
        }
    }
}
