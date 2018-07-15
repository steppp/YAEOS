#ifndef SCHEDULER
#define SCHEDULER
#include <pcb.h>

/* Stops the running process and puts it in the readyQueue
 * Loads the first process in the ready queue and runs it
 */
void dispatch();

/* Gets called when there are no more ready processes and handles all possibilities*/
void noMoreReadyPcbs();

/* Increases the priority of all processes in the queue pointed by p by 1 */
void increasePriority(pcb_t *p, void *arg);

/* Suspends the running process, saving its state in its pcb, and returns its address
 * Returns NULL if no processes are running
 * WARNING a dispatch should be called after this function, otherwise the processor will continue to
 * execute the previous code but the running process pointer won't point to the right process
 */
pcb_t *suspend();

#endif // SCHEDULER
