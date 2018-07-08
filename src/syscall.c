#include <scheduler.h>
#include <uARMtypes.h>
#include <uARMconst.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <tree.h>
#include <semaphore.h>
#include <pcbFree.h>
#include <main.h>
#include <types.h>

void P(int *semaddr)
{
    /*
        Retrieve the semaphore address from register a2
        Check if the value of the semaphore is >0
        If it is, decrease it and return
        Otherwise, take the running process' pcb and insert it in the semaphore queue and call a
        scheduler dispatch
    */
    (*semaddr)--;
    if(*semaddr < 0)
    {
        pcb_t *p;   /* holds the former running pcb pointer*/
        p = suspend();
        insertBlocked(semaddr,p);
        dispatch();
    }
}

void V(int *semaddr)
{
    (*semaddr)++;
    if(*semaddr <= 0)
    {
        pcb_t *p;   /* holds the unblocked pcb */
        p = removeBlocked(semaddr);
        if (p != NULL)
        {
            insertProcQ(&readyQueue,p);
            readyPcbs++;
        }
    }
}

/* SYSCALL 1, Tries to allocate a new process, if succesful configures it with the parameters and returns 0, else returns -1*/
int createProcess(state_t *statep, int priority, void **cpid){
	pcb_t *newproc= allocPcb();
	if ( newproc != NULL ){
		newproc->p_parent=runningPcb;
		newproc->p_s=*statep;
		newproc->old_priority=newproc->p_priority=priority;
		*cpid=newproc;
        newproc->waitingOnIO = 0;
        newproc->waitingForChild = 0;
        activePcbs++;
        insertProcQ(&readyQueue,newproc);
        readyPcbs++;
		return 0;
	}
	else{
		//You can't create a new Process, return -1
		return -1;
	}
}

/* Helping function for SYSCALL2, it recursievly kills all the children of a process*/
void killProcessSubtree(pcb_t *pcb){
	if(pcb == runningPcb) runningPcb = NULL;
	pcb_t *child;
	while ((child=removeChild(pcb)) !=NULL) killProcessSubtree(child);

    // resume the parent process if it has been suspended using the SYS10
    if (pcb->p_parent->waitingForChild) {       // if the parent is waiting for a child to terminate
        V(pcb->p_parent->p_semKey);             // unblock the parent process
        pcb->p_parent->waitingForChild = 0;     // the parent is no longer waiting for a child to terminate
    }

    if (outProcQ(&readyQueue,pcb) != NULL) /* removing it from ready queue*/
        readyPcbs--;
    if (pcb->waitingOnIO)   /* Process was blocked on I/O */
        softBlockedPcbs--;
    else
    {
        if (pcb->p_semKey != NULL) /* Process was blocked on a nondevice semaphore */
            (*(pcb->p_semKey))++;       /* The value of the semaphore needs to be adjusted*/
        activePcbs--;
    }

    outChildBlocked(pcb); /* removing the pcb from any queue it's blocked on */
    outChild(pcb);  /* Orphaning pcb */
	freePcb(pcb);
}

/* SYCALL 2 , kills the process and all its childs
 * What to do if it kills the running Process? It calls dispatch
 * How to check if it kills the running Process? It uses the global variable runningPcb , setting it to NULL if it kills it
 */
int terminateProcess(void * pid){
	if(pid==NULL) pid=runningPcb;
	killProcessSubtree(pid);
	if (runningPcb==NULL) dispatch();
	return 0;
}

/* SYSCALL 5, specifies what handlers to use, depending by trap.
 * When called a trap, the state of the calling process will copied in the 'old' area
 * and will loaded the state in the 'new' area.
 * Can be set only once a time for a process
 * Types can be:
    - 0 (SYSCALL/breakpoint)
    - 1 (TLB trap)
    - 2 (Program trap)
 * Returns 0 in case of success, -1 in case of failure.
*/
int specifyTrapHandler(int type, state_t *old, state_t *new) {
  switch (type) {
    case 0:
        //if currentProcess' areas are clean
        if (runningPcb->sysbk_new == NULL && runningPcb->sysbk_old == NULL) {
            //set areas
            runningPcb->sysbk_new = new;
            runningPcb->sysbk_old = old;
        }
        //else if are not clean are already set, failure! Return -1
        else {
            return -1;
        }
        break;

    case 1:
        //if currentProcess' areas are clean
        if (runningPcb->tlb_new == NULL && runningPcb->tlb_old == NULL) {
            //set areas
            runningPcb->tlb_new = new;
            runningPcb->tlb_old = old;
        }
        //else if are not clean are already set, failure! Return -1
        else {
            return -1;
        }
        break;

    case 2:
        //if currentProcess' areas are clean
        if (runningPcb->pgmtrap_new == NULL && runningPcb->pgmtrap_old == NULL) {
            //set areas
            runningPcb->pgmtrap_new = new;
            runningPcb->pgmtrap_old = old;
        }
        //else if are not clean are already set, failure! Return -1
        else {
            return -1;
        }
        break;

    default:
      return -1;
  }
  return 0; //Success!
}

/* SYSCALL 6, returns times of current running processes
 * user contains the time spent in user mode of the process
 * kernel contains the time spent in kernel mode of the process
 * wallclock contains the time from first process load.
 */

void getTimes(cpu_t *user, cpu_t *kernel, cpu_t *wallclock) {
  user = &(runningPcb->usertime);
  kernel = &(runningPcb->kerneltime);
  wallclock = &(runningPcb->wallclocktime);
}

/* SYSCALL 7: Stops the current running process and adds it to the pseudoClockSem*/

void waitForClock(){

    pcb_t *p;   // Will hold the current running pcb
    p = suspend();
    if (p !=NULL) P( &pseudoClockSem, p);
    dispatch();

}

/*
 * SYSCALL 9
 * Returns this process' PID and its father's one
 * If pid or ppid is NULL, the associated PID is not returned
 * Returns NULL as the ppid if this is the root process
 */
void getPids(void **pid, void **ppid) {
    if (ppid != NULL) {
        if (runningPcb->p_parent != NULL)
            *ppid = runningPcb->p_parent;
        else *ppid = NULL;
    }

    if (pid != NULL)
        *pid = runningPcb;
}

/*
 * SYSCALL 10
 * Puts the process in a suspended state waiting for a child to finish its execution
 */
void waitChild() {
    int c_sem = 1;  // verificare che questa dichiarazione non possa verificare casi di dangling references
    runningPcb->waitingForChild = 1;    /* This will be used later to tell wether this process were waiting for a child to end  */
    P(&c_sem);                          /* Suspend the process */

    /* perform extra work when the process resumes  */
}
