#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <uARMconst.h>
#include <uARMtypes.h>

#include  <const.h>
#include  <pcb.h>

/*
 * Design choices:
 * 
 * - The chosen hash function is the multiplicative hash. Despite the computational cost due to the
 * multiplication, this function has the pro of being independent of the size of the hash table. The
 * constant used for the multiplication was suggested by Donald Knuth and should be good in most
 * situations.
 * - The queue of PCBs waiting on a semaphore is a priority queue, where the priority is given by
 * the PCBs' priority
 * - The ASHT is a chained hash table with linked lists
 */

#define HASH_MULT_CONST 0.6180339887498948

typedef struct semd_t
{
    struct semd_t *s_next;
    int *s_key;
    struct pcb_t *s_procQ;
} semd_t;

extern semd_t semd_table[MAXSEMD];
/* Head of the list of free semaphore descriptors */
extern semd_t *semdFree_h;
/* Semaphores descriptors hash table */
extern semd_t *semdhash[ASHDSIZE];

/* 
 * Inserts the PCB referenced by p in the queue of blocked processes of the semaphore descriptor with the given
 * key.  If the table contains no semaphore descriptor with the given key, a new semaphore descriptor is allocated
 * from the list of free semds and is inserted the the table.  If it's not possible to allocate a new semd, returns
 * -1; if the given PCB is already blocked on a semaphore returns -2; otherwise, returns 0.
 */
int insertBlocked(int *key,pcb_t *p);

/*
 * Returns a pointer to the PCB of the first blocked process on the semaphore referenced by the given key, without
 * dequeing it.
 * If the semaphore with referenced by the given key does not exist, returns NULL.
 */
pcb_t *headBlocked(int *key);

/*
 * Returns a pointer to the PCB of the first blocked process on the semaphore referenced by the given key,
 * dequeing it.
 * If the semaphore with referenced by the given key does not exist, returns NULL.
 * If the processes queue of the semaphore referenced by key becomes empty, removes the corresponding semaphore
 * descriptors from the hashtable and inserts it in the free semd queue.
 */
pcb_t *removeBlocked(int *key);

/* Calls the function fun on all the processes blocked on the semaphore referenced by key. */
void forallBlocked(int *key, void fun(pcb_t *pcb, void *), void *arg);

/*
 * Removes the PCB pointed by p from the queue of the semaphore where p is blocked. Hash table will be updated
 * accordingly.
 */
void outChildBlocked(pcb_t *p);

/* Initializes the semdFree queue so that all the elements of semdTable are in it */
void initASL();

#endif
