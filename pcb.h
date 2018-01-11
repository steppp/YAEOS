#define MAXPROC 10

//pcb_t struct as defined in specifications
typedef struct pcb_t {
	struct pcb_t *p_next;
	struct pcb_t *p_parent;
	struct pcb_t *p_first_child;
	struct pcb_t *p_sib;
	state_t p_s;
	int priority;
	int *p_semKey;
} pcb_t
