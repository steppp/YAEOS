//for semaphores

int 	insertBlocked(int *key,pcb_t *p);
pcb_t 	*headBlocked(int *key);
pcb_t	*removeBlocked(int  *key);
void 	forallBlocked(	int *key, 
			void fun(pcb_t *pcb, void *), 
			void *arg);
outChildBlocked(pcb_t *p);
void 	initASL();
