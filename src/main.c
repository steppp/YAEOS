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

int pseudoClockSem;  /* Semaphore for the pseudoclock. */

unsigned int readyPcbs; /* Number of ready processes */
unsigned int softBlockedPcbs; /* Number of processes waiting on a I/O operation */
unsigned int activePcbs; /* Number of active processes (not waiting on an I/O operation) */

cpu_t clockStartLO;
cpu_t clockStartHI;

timcause_t lastTimerCause; /* Last timer interrupt's cause */

extern void test();

void initFirstPCB() {
    state_t p_s = {
        .sp = RAM_TOP - FRAMESIZE,
        .pc = (memaddr) test,
        .cpsr = STATUS_ALL_INT_ENABLE(STATUS_SYS_MODE),     // kernel mode
        .CP15_Control = CP15_CONTROL_NULL                   // VM disabled
    };

    createProcess(&p_s, 0, NULL);
}

void initVars() {

    readyPcbs = 0;
    activePcbs = 0;
    softBlockedPcbs = 0;

    runningPcb = NULL;
    readyQueue = NULL;

    int i, j;

    pseudoClockTicks = 0;
    agingTicks = 0;
    pseudoClockSem = 0;

    for (i = 0; i < N_INTERRUPT_LINES - 4; i++)
        for (j = 0; j < DEV_PER_INT; j++)
            normalDevices[i][j] = 0;

    for (i = 0; i < DEV_PER_INT; i++)
        terminals[DEV_PER_INT][0] = terminals[DEV_PER_INT][1] = 0;

    clockStartLO = getTODLO();
    clockStartHI = getTODHI();
}

void initHandler(memaddr addr, memaddr handler) {
    state_t* new_state = (state_t *) addr;

    new_state->pc = handler;
    new_state->sp = RAM_TOP;
    new_state->cpsr = STATUS_ALL_INT_DISABLE(STATUS_SYS_MODE);
}

void initDataStructures() {
    initPcbs(); 
    initASL();
}

void init() {
    // init NEW areas for interrupts and traps
    initHandler(INT_NEWAREA, (memaddr) interruptHandler);
    initHandler(TLB_NEWAREA, (memaddr) tlbHandler);
    initHandler(PGMTRAP_NEWAREA, (memaddr) pgmTrapHandler);
    initHandler(SYSBK_NEWAREA, (memaddr) sysHandler);

    // init PHASE1's data structures
    initDataStructures();

    // init nucleus variables
    initVars();

    initFirstPCB();
}


int main() {
    init();

    dispatch(NULL);
    // end of the nucleus initialization

    return 0;
}

#ifdef DEBUG
void debug() {}
int debug1;
#endif // DEBUG
