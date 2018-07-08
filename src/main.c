#include <main.h>
#include <types.h>
#include <pcb.h>

// ready processes list
pcb_t *readyQueue;
// Current running process
pcb_t *runningPcb;
// list of proceses waiting for clock
pcb_t *waitingQueue;

int pseudoClockSem;  /* Semaphore for the pseudoclock. */

unsigned int readyPcbs; /* Number of ready processes */
unsigned int softBlockedPcbs; /* Number of processes waiting on a I/O operation */
unsigned int activePcbs; /* Number of active processes (not waiting on an I/O operation) */

timcause_t lastTimerCause; /* Last timer interrupt's cause */
cpu_t timeSliceTimer; /* Time remaining before the next timeSlice interrupt in microseconds */
cpu_t agingTimer; /* Time remaining before the next aging interrupt in microseconds */
cpu_t pseudoClockTimer; /* Time remaining before the next pseudoclock timer in microseconds */
