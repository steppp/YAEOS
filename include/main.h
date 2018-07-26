#ifndef MAIN_H
#define MAIN_H
#include <types.h>
#include <arch.h>
#include <pcb.h>

#define PSEUDOCLOCKPERIOD 100000UL      /* Pseudoclock period in microseconds */
#define AGINGPERIOD 10000UL             /* Aging period in microseconds */
#define TIMESLICEPERIOD 3000UL          /* Timeslice period in microseconds */

#define BYTELEN 8

#define TRANSM 1                        /* terminal related */
#define RECV 0

#define GET_STATUS_MODE(mode) ((mode) & 0x1F)


extern pcb_t *readyQueue;               /* Ready processes list */

extern pcb_t *runningPcb;               /* Current running process */

extern int pseudoClockSem;              /* Semaphore for the pseudoclock. */

extern unsigned int readyPcbs;          /* Number of ready processes */
extern unsigned int softBlockedPcbs;    /* Number of processes waiting on a I/O operation */
extern unsigned int activePcbs;         /* Number of active processes (not waiting on an I/O operation) */

extern timcause_t lastTimerCause;       /* Last timer interrupt's cause */
extern unsigned int pseudoClockTicks;   /* Number of times the pseudoclock caused an interrupt */
extern unsigned int agingTicks;         /* Number of times the aging caused an interrupt */
extern cpu_t clockStartLO;              /* TODLO of the moment the inizialization is complete */
extern cpu_t clockStartHI;              /* TODHI of the moment the inizialization is complete */

/* Device semaphores */

/*
 *  These devices have only one semaphore associated with them
 *  They are the devices of the categories other than Interval Timer (which is a single device),
 *  Terminals (which have two semaphore associated with each of them) and the two unused categories
 *  (IL_IPI and IL_CPUTIMER)
 */
extern int normalDevices[N_INTERRUPT_LINES - 4][DEV_PER_INT];
/*
 *  Terminals are treated differently because they are double devices (they behave like two separate
 *  devices, 1 for input and the other for output)
 */
extern int terminals[DEV_PER_INT][2];

#endif // MAIN_H
