#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <uARMconst.h>
#include <uARMtypes.h>

#include  <const.h>
#include  <pcb.h>

#define HASH_MULT_CONST 0.6180339887498948

/*
   Constant for the multiplcative hash; suggested by D.Knuth, should be good in most situations
*/

typedef struct semd_t
{
    struct semd_t *s_next;
    int *s_key;
    struct pcb_t *s_procQ;
} semd_t;

semd_t semd_table[MAXSEMD];
semd_t *semdFree_h;
// Head of the list of free semaphore descriptors
semd_t *semdhash[ASHDSIZE];
// Semaphores descriptors hash table

int insertBlocked(int *key,pcb_t *p);
/* 
   Inserts the PCB referenced by p in the queue of blocked processes of the semaphore descriptor with the given key.
    If the table contains no semaphore descriptor with the given key, a new semaphore descriptor is allocated from the
    list of free semds and is inserted the the table.
    If it's not possible to allocate a new semd, returns -1; otherwise, returns 0.
*/

pcb_t *headBlocked(int *key);
/*
   Returns a pointer to the PCB of the first blocked process on the semaphore referenced by the given key, without
   dequeing it.
   If the semaphore with referenced by the given key does not exist, returns NULL.
*/

pcb_t *removeBlocked(int  *key);
/*
   Returns a pointer to the PCB of the first blocked process on the semaphore referenced by the given key,
   dequeing it.
   If the semaphore with referenced by the given key does not exist, returns NULL.
   If the processes queue of the semaphore referenced by key becomes empty, removes the corresponding semaphore
   descriptors from the hashtable and inserts it in the free semd queue.
*/

void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg);
/*
   Calls the function fun on all the processes blocked on the semaphore referenced by key.
*/

void outChildBlocked(pcb_t *p);
/*
   Removes the PCB pointed by p from the queue of the semaphore where p is blocked. Hash table will be updated
   accordingly.
*/

void initASL();
/*
   Initialize the semdFree queue so that all the elements of semdTable are in it
*/

int hash(int* key);
/*
   Returns the hash address for the given key
*/

semd_t *hashentry(semd_t *bucketlist,int *key);
/*
   Given a hashtable bucketlist, returns the address of the entry containing the semaphore identified by key if present,
   NULL otherwise
*/

void enqueue(pcb_t **queue,pcb_t *p);
/*
   Enqueues the given PCB p into the given queue
*/

void removeEntry(semd_t **bucketlist,semd_t *entry);
/*
   Removes the given entry from the given bucketlist, if present
*/

void callFun(pcb_t *p,void (*fun)(pcb_t *pcb, void *),void *arg);
/*
   Given a list of pcb, applies the function fun with the given argument to all the pcbs of the list
*/

semd_t *removePCB(pcb_t *p,int *ind);
/*
   Removes the given PCB p from the bucketlists of index greater or equal to the value pointed by ind and returns the
   correponding semd entry, if present in semdhash; otherwise returns NULL
*/

semd_t *removePCBfromQueue(pcb_t *p,semd_t *bucketlist);
/*
   Removes the given PCB p from the given bucketlist returns the correponding semd entry, if present; otherwise returns NULL
*/

void fillFreeSemd(int i);
/*
   Adds all the semaphore descriptors in semdtable with index greater or equalt to i to the freeSemd list
*/

#endif
