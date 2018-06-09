#ifndef SCHEDULER
#define SCHEDULER
#include <pcb.h>

// ready processes list
extern pcb_t *readyQueue;
// Current running process
extern pcb_t *runningPcb;

// Number of ready processes
extern unsigned readyPcbs;
// Number of processes waiting on a I/O operation
extern unsigned softBlockedPcbs;
// Number of processes waiting on a semaphore
extern unsigned activePcbs;

void dispatch();
// Stops the running process and puts it in the readyQueue
// Loads the first process in the ready queue and runs it

int checkForDeadlock();
// Returns 1 if the system is in a deadlocked state, 0 otherwise

pcb_t *suspend();
// Suspends the running process, saving its state in its pcb, and returns its address
// Returns NULL if no processes are running
// WARNING a dispatch should be called after this function, otherwise the processor will continue to
// execute the previous code but the running process pointer won't point to the right process

#endif // SCHEDULER
