#ifndef PCB_H
#define PCB_H

#include <uARMtypes.h>
#include <types.h>
#include <uARMconst.h>
#include  <const.h>


/* pcb_t struct as defined in specifications */

typedef struct pcb_t {
	struct pcb_t *p_next;           /* Process' successor in the list it's in*/
	struct pcb_t *p_parent;         /* Process' parent */
	struct pcb_t *p_first_child;    /* Process' first child */
	struct pcb_t *p_sib;            /* Process' right sibling */
	state_t p_s;                    /* Processor's state */
	int p_priority;                 /* Current priority */
    int old_priority;               /* Original priority, to restore after aging*/
	int *p_semKey;                  /* Semaphore the process is blocked on */
    int waitingOnIO;                /* Used to count the number of softblocked processes */
    int childSem;                   /* Used to wait on a child */
	/* Process times */
    cpu_t usertime; 				/* Process' time spent in user mode */
    cpu_t kerneltime;				/* Process' time spent in kernel mode */
	cpu_t lasttime;					/* Last TOD marker. We use it for keep track of last activation/deactivation */
    cpu_t wallclocktime;			/* Process' TOD when starting. Used for calculating total wallclocktime  */
	/* Trap handlers */
	state_t *sysbk_new;
	state_t *sysbk_old;
	state_t *tlb_new;
	state_t *tlb_old;
	state_t *pgmtrap_new;
	state_t *pgmtrap_old;
} pcb_t;


/*
 * 
 */
pcb_t *pcbFree_h;

/*
 * 
 */
pcb_t pcbFree_table[MAXPROC];


/*
 * Initializes the list so it contains all the elements of pcbFree_table.
 * It has to be called only one time, when the data is being initialized.
 */
void initPcbs();

/* Inserts the pointed PCB passed by parameted in the freePCB list */
void freePcb(pcb_t *p);

/*
 * Removes a pcb from the list of free pcbs and returns it.
 * The returned pcb has all fields set to 0/NULL.
 * If the free pcbs list is empty the function returns NULL.
 */
pcb_t *allocPcb();

#endif //PCB_H
