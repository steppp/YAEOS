#ifndef SYSCALL_H
#define SYSCALL_H

/* This file contains descriptions for the use of the functions.
 * To see how they work in detail please consult the corresponding .c file,
 * or check the documentation.
 */


/* SYSCALL 1, Tries to allocate a new process, if succesful configures it with the parameters and returns 0, else returns -1*/
int createProcess(state_t *statep, int priority, void **cpid);

/* Helping function for SYSCALL2, it recursievly kills all the children of a process*/ 
void killProcessSubtree(pcb_t *pcb);

/* SYCALL 2 , kills the process and all its childs
 * If the process terminated is the current running Process it calls dispatch.
 */
int terminateProcess(void * pid);

/* SYSCALL 3 , calls a P on the semaphore specified in semaddr */
void P(int *semaddr);

/* SYSCALL 4 , calls a V on the semaphore specified in semaddr 
 * Given that the runningProcess may be suspended, its current state is passed an argument to be
 * saved if needed
 */
void V(int *semaddr, state_t *to_save);


/* SYSCALL 5, specifies what handlers to use, depending by trap.
 * When called a trap, the state of the calling process will copied in the 'old' area
 * and will loaded the state in the 'new' area.
 * Can be set only once a time for a process
 * Types can be:
 *  - 0 (SYSCALL/breakpoint)
 *  - 1 (TLB trap)
 *  - 2 (Program trap)
 * Returns 0 in case of success, -1 in case of failure.
*/
int specifyTrapHandler(int type, state_t *old, state_t *new);

/* SYSCALL 6, returns times of current running processes
 * If a field is set to 0 means that are not required to caller so will not returned
 * user contains the time spent in user mode of the process
 * kernel contains the time spent in kernel mode of the process
 * wallclock contains the time from first process load.
 */
void getTimes(cpu_t *user, cpu_t *kernel, cpu_t *wallclock);


/* SYSCALL 7, Stops the current running process and adds it to the pseudoClockSem*/
void waitForClock();


/*  Helper function for syscall 8 , given a register it calculates both its interrupt Line and its device number
 *  If the interrupt Line is a terminal, it calculates if its a Recv or a Trasm command
 *  the intLine, devNo and termIO parameters are used to return values
 */
void getDeviceFromRegister(int * intLine , int * devNo, int * termIO, unsigned int *comm_device_register);

/*  SYSCALL 8,Activates the I/O operations copying the command in the appropriate command device register
 *  the caller will be suspended and appended to the appropiate semaphore 
 *  (using a normalDevice semaphore if the interupt Line is <7 or a terminal semaphone if =7)
 */
unsigned int ioOperation(unsigned int command, unsigned int *comm_device_register);

/* SYSCALL 9, Returns this process' PID and its father's one
 * If pid or ppid is NULL, the associated PID is not returned
 * Returns NULL as the ppid if this is the root process
 */
void getPids(void **pid, void **ppid);

/* SYSCALL 10,Puts the process in a suspended state waiting for a child to finish its execution */
void waitChild();

/* Hander for program Traps */
void pgmTrapHandler();

/* Handler for tlb */
void tlbHandler();

/* Handler for Syscalls/breakpoints */
void sysHandler();

#endif // SYSCALL_H
