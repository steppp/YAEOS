#include <scheduler.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <semaphore.h>

void P()
{
    /*
        Retrieve the semaphore address from register a2
        Check if the value of the semaphore is >0
        If it is, decrease it and return
        Otherwise, take the running process' pcb and insert it in the semaphore queue and call a
        scheduler dispatch
    */
    state_t curstate;
    pcb_t *p;
    int *semaddr;
    STST(&curstate);
    *semaddr = curstate.a2;
    (*semaddr)--;
    if(*semaddr < 0)
    {
        p = runningPcb;
        runningPcb = NULL;
        insertBlocked(semaddr,p);
        dispatch();
    }
}

void V()
{
    state_t curstate;
    pcb_t *p;
    int *semaddr;
    STST(&curstate);
    *semaddr = curstate.a2;
    (*semaddr)++;
    if(*semaddr <= 0)
    {
        p = removeBlocked(semaddr);
        if (p != NULL)
            insertProcQ(&readyQueue,p);
    }
}
