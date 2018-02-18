//#define MAXPROC 10
#ifndef PCB_H
#define PCB_H
#include "state.h"

//pcb_t struct as defined in specifications
typedef struct pcb_t {
	struct pcb_t *p_next;
	struct pcb_t *p_parent;
	struct pcb_t *p_first_child;
	struct pcb_t *p_sib;
	state_t p_s;
	int p_priority;
	int *p_semKey;
} pcb_t;

#endif //PCB_H
