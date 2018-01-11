//for lists

void 	insertProcQ(pcb_t **head, pcb *p);
pcb_t 	*headProcQ(pcb_t *head);
pcb_t 	*removeProcQ(pcb_t **head);
pcb_t 	*outProcQ(pcb_t **head, pcb_t *p);
void 	forallProcQ(	pcb_t *head, 
			void fun(pcb_t *pcb, void *),
			void *arg);


