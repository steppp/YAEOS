#ifndef SCHEDULER
#define SCHEDULER
#include <pcb.h>

// ready processes list
extern pcb_t *readyQueue;
// Current running process
extern pcb_t *runningPcb;

extern unsigned int readyPcbs; /* Number of ready processes */
extern unsigned int softBlockedPcbs; /* Number of processes waiting on a I/O operation */
extern unsigned int activePcbs; /* Number of active processes (not waiting on an I/O operation) */

// Stops the running process and puts it in the readyQueue
// Loads the first process in the ready queue and runs it
void dispatch();

// Returns 1 if the system is in a deadlocked state, 0 otherwise
int checkForDeadlock();

// Suspends the running process, saving its state in its pcb, and returns its address
// Returns NULL if no processes are running
// WARNING a dispatch should be called after this function, otherwise the processor will continue to
// execute the previous code but the running process pointer won't point to the right process
pcb_t *suspend();

#endif // SCHEDULER
