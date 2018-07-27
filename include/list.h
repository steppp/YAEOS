#ifndef LIST_H
#define LIST_H

#include <uARMconst.h>
#include <uARMtypes.h>

#include  <pcb.h>


/* Inserts a new pcb into the queue, the position varies depending on the "p_priority" field of the pcb (Descending order) */
void 	insertProcQ(pcb_t **head, pcb_t *p);

/* Returns the first element of the queue without removing it , returns NULL if the queue is empty */
pcb_t 	*headProcQ(pcb_t *head);

/* Removes and returns the first element of the queue, returns NULL if the queue is empty */
pcb_t 	*removeProcQ(pcb_t **head);

/* Removes and returns the specified pcb (p) from the queue , returns NULL if the queue is empty or if p isn't in the queue */
pcb_t 	*outProcQ(pcb_t **head, pcb_t *p);

/* Calls the "fun" function for all elements of the queue */
void 	forallProcQ(pcb_t *head, 
			void fun(pcb_t *pcb, void *),
			void *arg);

#endif //LIST_H

