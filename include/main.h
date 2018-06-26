#ifndef MAIN_H
#define MAIN_H
#include <types.h>
#include <pcb.h>

#define PSEUDOCLOCKPERIOD 100000UL  /* Pseudoclock period in microseconds */
#define AGINGPERIOD 10000UL /* Aging period in microseconds */
#define TIMESLICEPERIOD 3000UL /* Timeslice period in microseconds */

#define BYTELEN 8
#define DEVICES 8   /* number of devices */

// ready processes list
extern pcb_t *readyQueue;
// Current running process
extern pcb_t *runningPcb;

extern int pseudoClockSem;  /* Semaphore for the pseudoclock. */

extern unsigned int readyPcbs; /* Number of ready processes */
extern unsigned int softBlockedPcbs; /* Number of processes waiting on a I/O operation */
extern unsigned int activePcbs; /* Number of active processes (not waiting on an I/O operation) */

extern timcause_t lastTimerCause; /* Last timer interrupt's cause */
extern int pseudoClockTicks;    /* Number of times the pseudoclock caused an interrupt */
extern int agingTicks;  /* Number of times the aging caused an interrupt */
extern cpu_t clockStart; /* TOD of the moment the inizialization is complete */

/* Device semaphores */

extern int disks[DEVICES];
extern int tapes[DEVICES];
extern int networks[DEVICES];
extern int printers[DEVICES];
extern int terminals[DEVICES][2];

#endif // MAIN_H
