#include <scheduler.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <semaphore.h>

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
        activePcbs++;
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
            activePcbs++;
            insertProcQ(&readyQueue,p);
            readyPcbs++;
        }
    }
}
