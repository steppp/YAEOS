#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <uARMtypes.h>
#include <types.h>

/* Extracts the line from which the interrupt came and the device number of the one who caused it.
 * Calculates the appropriate device register address and calls the appropriate handler routine,
 * giving it the device register address as an argument */
void interruptsDispatcher();

/* Timer's Interrupt Service Routine. Intercepts the interrupt, determines the cause, takes the
 * apprioriate action and updates the intervale timer */
int handleTimer();

/* Updates the timer with the minimum between timeSlice interval and pseudoclock deadline */
void updateTimer();

void interruptHandler();

#endif // INTERRUPTS_H
