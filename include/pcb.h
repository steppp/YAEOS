#ifndef PCB_H
#define PCB_H
#include <uARMtypes.h>
#include <types.h>

//pcb_t struct as defined in specifications

typedef struct pcb_t {
	struct pcb_t *p_next;   /* Process' successor in the list it's in*/
	struct pcb_t *p_parent; /* Process' parent */
	struct pcb_t *p_first_child; /* Process' first child */
	struct pcb_t *p_sib; /* Process' right sibling */
	state_t p_s;    /* Processor's state */
    int old_priority; /* Original priority, to restore after aging*/
	int p_priority; /* Current priority */
	int *p_semKey; /* Semaphore the process is blocked on */
    int waitingOnIO; /* Used to count the number of softblocked processes */
    cpu_t usertime;
    cpu_t kerneltime;
    cpu_t wallclocktime;
		//trap handlers
		state_t *sysbk_new;
		state_t *sysbk_old;
		state_t *tlb_new;
		state_t *tlb_old;
		state_t *pgmtrap_new;
		state_t *pgmtrap_old;
} pcb_t;

#endif //PCB_H
