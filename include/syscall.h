#ifndef SYSCALL_H
#define SYSCALL_H

void P(int *semaddr);

void V(int *semaddr);

/* SYSCALL 1, Tries to allocate a new process, if succesful configures it with the parameters and returns 0, else returns -1*/
int createProcess(state_t *statep, int priority, void **cpid);

/* Helping function for SYSCALL2, it recursievly kills all the children of a process*/ 
void killProcessSubtree(pcb_t *pcb);

/* SYCALL 2 , kills the process and all its childs
 * What to do if it kills the running Process? It calls dispatch
 * How to check if it kills the running Process? It uses the global variable runningPcb , setting it to NULL if it kills it
 */
int terminateProcess(void * pid);

/* SYSCALL 5, specifies what handlers to use, depending by trap.
 * When called a trap, the state of the calling process will copied in the 'old' area
 * and will loaded the state in the 'new' area.
 * Can be set only once a time for a process
 * Types can be:
    - 0 (SYSCALL/breakpoint)
    - 1 (TLB trap)
    - 2 (Program trap)
 * Returns 0 in case of success, -1 in case of failure.
*/
int specifyTrapHandler(int type, state_t *old, state_t *new);

/* SYSCALL 6, returns times of current running processes
 * user contains the time spent in user mode of the process
 * kernel contains the time spent in kernel mode of the process
 * wallclock contains the time from first process load.
 */

void getTimes(cputtime_t *user, cputtime_t *kernel, cputtime_t *wallclock);


/* SYSCALL 7: Stops the current running process and adds it to the pseudoClockSem*/
void waitForClock();

/*
 * SYSCALL 9
 * Returns this process' PID and its father's one
 * If pid or ppid is NULL, the associated PID is not returned
 * Returns NULL as the ppid if this is the root process
 */
void getPids(void **pid, void **ppid);

/*
 * SYSCALL 10
 * Puts the process in a suspended state waiting for a child to finish its execution
 */
void waitChild();

#endif // SYSCALL_H
