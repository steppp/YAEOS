/*
 *  Questo header descrive lo stato di un processo, che
 *  deve essere salvato per poter ripristinare l'esecuzione
 *
 * */

#define STATE_H

#ifndef STATE_H
#define STATE_H

typedef enum state_t {
    ready = 0,
    waiting = 1,
    running = 2
} state_t;

#endif
