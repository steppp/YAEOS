#ifndef SCHEDULER
#define SCHEDULER
#include <pcb.h>


/* This file contains descriptions for the use of the functions.
 * To see how they work in detail please consult the corresponding .c file,
 * or check the documentation.
 */

/*
 *  Stops the running process and puts it in the readyQueue, if there is a running process
 *  Loads the first process in the ready queue and runs it
 */
void dispatch(state_t *to_save);

/* Increases the priority a process with p as a pcb by 1, if less than MAXPRIO */
void increasePriority(pcb_t *p, void *arg);

/*  Suspends the running process and returns its address
 *  Returns NULL if no processes are running
 *  WARNING a dispatch should be called after this function, at some point
 */
pcb_t *suspend();

/* Restores the running process, given the old area, adjusting the pc as necessary */
void restoreRunningProcess(state_t *oldarea);

/*
 *  Inserts a new process in the ready queue. If the new process has a greater priority than the
 *  current running process, the currently running process is suspended and the state of the process,
 *  passed in the variable to_save, is saved in its pcb. This takes into account that if the cause
 *  of the exception was a device interrupt the pc register must be decreased by 4
 *  Returns 0 on successful insertion, -1 on failure
 */
int insertInReady(pcb_t *p, state_t *to_save);


/*
 *  This facility updates the time spent for the current running process in user mode.
 *  Substract the time gave from parameters (Hi and Low part of TOD)
 *  to the last time marked (with freezeLastTime) in the process, 
 *  and then adds it to the total user time.
 */
void userTimeAccounting(unsigned int TOD_Hi, unsigned int TOD_Low);

/*
 *  This facility updates the time spent for the argument-passed process in kernel mode.
 *  Substract the time gave from parameters (Hi and Low part of TOD)
 *  to the current TOD
 *  and then adds it to the total kernel time.
 *  If the process passed by argument is NULL nothing is updated.
 */
void kernelTimeAccounting(unsigned int TOD_Hi, unsigned int TOD_Low, pcb_t* process);


/*
 *  This facility freezes the lasttime variable to the current TOD.
 *  It's used for calculating the user time.
 */
void freezeLastTime(pcb_t *p);

/*
 *  Handles the passup of a sysbk/tlb/pgmtrap. The argument is the old area where the state of the
 *  running process has been saved. Returns 0 if the passup can't be done (runningPcb == NULL or
 *  SYS5 not previously called), doesn't return and realizes the passup otherwise
 */
int passup(state_t *old_to_save);

#endif // SCHEDULER
