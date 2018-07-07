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
