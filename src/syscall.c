#include <scheduler.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <semaphore.h>
#include <pcbFree.h>

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
        readyPcbs--;
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

int createProcess(state_t *statep, int priority, void **cpid){
	pcb_t *newproc= allocPcb();
	if ( newproc != NULL ){
		newproc->p_parent=runningPcb;
		newproc->p_s=*statep;
		newproc->old_priority=newproc->p_priority=priority;
		*cpid=newproc;
		//TODO: Se aggiungiamo WaitingProcess, bisogna modificare, rimuovere questo commento prima di sacrificarlo a Davoli
		return 0;

	}
	else{
		//You can't create a new Process, return -1
		return -1;
	}
}
