#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <uARMtypes.h>
#include <types.h>

/* Timer's Interrupt Service Routine. Intercepts the interrupt, determines the cause, takes the
 * apprioriate action and updates the intervale timer */
int handleTimer();

/* Updates the timer with the minimum between timeSlice interval and pseudoclock deadline */
void updateTimer();

/*
    General Interrupt Service Routine. Determines what's the cause of the interrupt and executes the
    appropriate actions.
 */
void interruptHandler();

#endif // INTERRUPTS_H
