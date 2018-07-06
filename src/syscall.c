#include <scheduler.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <tree.h>
#include <semaphore.h>
#include <pcbFree.h>
#include <main.h>

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
 * Types can be:
    - 0 (SYSCALL/breakpoint)
    - 1 (TLB trap)
    - 2 (Program trap)
 * Returns 0 in case of success, -1 in case of failure.
*/
int specifyTrapHandler(int type, state_t *old, state_t *new) {
  state_t *trans_old, *trans_new;
  switch (type) {
    case 0:
        trans_new = SYSBK_NEWAREA;
        trans_old = SYSBR_OLDAREA;
      break;

    case 1:
        trans_new = TLB_NEWAREA;
        trans_old = TLB_OLDAREA;
      break;

    case 2:
        trans_new = PGMTRAP_NEWAREA
        trans_old = PGMTRAP_OLDAREA
      break;

    default:
      return -1;
  }

  new = trans_new;
  old = trans_old;
  return 0;
}

/* SYSCALL 7: Stops the current running process and adds it to the waitingQueue, the list of all processes that are waiting for the clock*/

void waitForClock(){

    pcb_t *p;   // Will hold the current running pcb
    p = suspend();
    if (p !=NULL) insertProcQueue( &waitingQueue, p);
    dispatch();

}

/* Gets called after a pseudoclock tick, removes all processes from the waitingQueue and puts them back in the readyQueue */
void wakeUp(){
    pcb_t *p; // Placeholder pcb pointer    
    while ( (p=removeProcQ(&waitingQueue) != NULL ){
        insertProcQ(&readyQueue,p);
    }
}
