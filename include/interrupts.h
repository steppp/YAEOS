#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <uARMtypes.h>
#include <types.h>
#include <pcb.h>

/* This file contains descriptions for the use of the functions.
 * To see how they work in detail please consult the corresponding .c file,
 * or check the documentation.
 */


/*  Timer's Interrupt Service Routine. Intercepts the interrupt, determines the cause and takes the apprioriate action. 
 *  Returns 1 if a dispatch is necessary (this happens if the timeslice expires), 0 otherwise.
 */
int handleTimer();

/*  Updates the timer with the minimum between timeSlice interval, the pseudoclock deadline and the aging deadline */
void updateTimer();

/*
 * General Interrupt Service Routine. Determines what's the cause of the interrupt and executes the appropriate actions.
 * If the cause of the interrupt is a device from the device lines > 3 the interrupt is handled directly. Otherwise, an helper function is called
 */
void interruptHandler();

#endif // INTERRUPTS_H
