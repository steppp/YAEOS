#ifndef TYPES_H
#define TYPES_H
#include <uARMtypes.h>

typedef int devhdl_t(devreg_t*); /* Type of a function that takes a device register pointer as argument and
                                           returns int, for device handler routines */

typedef unsigned int cpu_t; /* cpu time type */

typedef enum {PSEUDOCLOCK, TIMESLICE, AGING} timcause_t; /* cause of interval timer interrupt type */

//don't know if this is the right place to define cputtime_t...
typedef int cputtime_t;

#endif // TYPES_H