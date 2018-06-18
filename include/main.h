#ifndef MAIN_H
#define MAIN_H
#include <types.h>
#include <pcb.h>

#define PSEUDOCLOCKPERIOD 100000UL
#define AGINGPERIOD 10000UL
#define TIMESLICEPERIOD 3000UL

#define BYTELEN 8

// ready processes list
extern pcb_t *readyQueue;
// Current running process
extern pcb_t *runningPcb;

extern int pseudoClockSem;  /* Semaphore for the pseudoclock. */

extern unsigned int readyPcbs; /* Number of ready processes */
extern unsigned int softBlockedPcbs; /* Number of processes waiting on a I/O operation */
extern unsigned int activePcbs; /* Number of active processes (not waiting on an I/O operation) */

extern devhdl_t *deviceHandlers[5]; /* Contains the addresses of the device handling routines */

extern timcause_t lastTimerCause; /* Last timer interrupt's cause */
extern cpu_t timeSliceTimer; /* Time remaining before the next timeSlice interrupt in microseconds */
extern cpu_t agingTimer; /* Time remaining before the next aging interrupt in microseconds */
extern cpu_t pseudoClockTimer; /* Time remaining before the next pseudoclock timer in microseconds */


#endif // MAIN_H
