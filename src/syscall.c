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
    pcb_t *p;
    (*semaddr)--;
    if(*semaddr < 0)
    {
        p = suspend();
        readyPcbs--;
        insertBlocked(semaddr,p);
        activePcbs++;
        dispatch();
    }
}

void V(int *semaddr)
{
    pcb_t *p;
    (*semaddr)++;
    if(*semaddr <= 0)
    {
        p = removeBlocked(semaddr);
        if (p != NULL)
        {
            activePcbs++;
            insertProcQ(&readyQueue,p);
            readyPcbs++;
        }
    }
}
