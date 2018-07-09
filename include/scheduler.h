#ifndef SCHEDULER
#define SCHEDULER
#include <pcb.h>

// Hander for program Traps
void pgmTrapHandler();

// Handler for tlb
void tlbHandler();

// Handler for Syscalls/breakpoints
void sysHandler();

// Stops the running process and puts it in the readyQueue
// Loads the first process in the ready queue and runs it
void dispatch();

// Returns 1 if the system is in a deadlocked state, 0 otherwise
int checkForDeadlock();

/* Increases the priority of all processes in the queue pointed by p by 1 */
void increasePriority(pcb_t *p, void *arg);

// Suspends the running process, saving its state in its pcb, and returns its address
// Returns NULL if no processes are running
// WARNING a dispatch should be called after this function, otherwise the processor will continue to
// execute the previous code but the running process pointer won't point to the right process
pcb_t *suspend();

#endif // SCHEDULER
