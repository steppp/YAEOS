#include <main.h>
#include <types.h>
#include <pcb.h>
#include <scheduler.h>
#include <syscall.h>
#include <stdlib.h>
#include <arch.h>
#include <uARMconst.h>
#include <interrupts.h>
#include <libuarm.h>
#include <asl.h>

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

extern void test();

void initFirstPCB() {
    state_t p_s = {
        .sp = RAM_TOP - FRAMESIZE,
        .pc = (memaddr) test,
        .cpsr = 0x1F,               // kernel mode
        .CP15_Control = 0           // VM disabled
    };

    pcb_t * pcb = malloc(sizeof(pcb_t));

    // here the third parameter should not be necessary
    // however the create process function might raise an error if
    // passing NULL as the third parameter
    createProcess(&p_s, 0, (void **) &pcb);
}

void initVars() {
    readyPcbs = 0;
    activePcbs = 0;
    softBlockedPcbs = 0;

    runningPcb = NULL;
    waitingQueue = NULL;

    timeSliceTimer = TIMESLICEPERIOD;
    agingTimer = AGINGPERIOD;
    pseudoClockTimer = PSEUDOCLOCKPERIOD;
}

void initHandler(memaddr addr, void handler()) {
    state_t* new_state = (state_t *) addr;
    STST(new_state);    // copy the current state into the struct

    new_state->pc = (memaddr) handler;
    new_state->sp = RAM_TOP;
    new_state->cpsr |= 0xC0;
}

void initDataStructures() {
    initPcbs(); 
    initASL();
}

void init() {
    setSTATUS(0xDF);

    // init NEW areas for interrupts and traps
    initHandler(INT_NEWAREA, interruptHandler);
    initHandler(TLB_NEWAREA, tlbHandler);
    initHandler(PGMTRAP_NEWAREA, pgmTrapHandler);
    initHandler(SYSBK_NEWAREA, sysHandler);

    // init PHASE1's data structures
    initDataStructures();

    initFirstPCB();

    // init nucleus variables
    initVars();

}


int main() {
    init();

    dispatch();
    // end of the nucleus initialization

    return 0;
}
