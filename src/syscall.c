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
		//TODO: Se aggiungiamo WaitingProcess, bisogna modificare, rimuovere questo commento prima di sacrificarlo a Davoli
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
    if (pcb->waitingOnIO)
        softBlockedPcbs--;
    else
        activePcbs--;
    outChildBlocked(pcb); /* removing the pcb from any queue it's blocked on */
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
