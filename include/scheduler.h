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

#endif // SCHEDULER
