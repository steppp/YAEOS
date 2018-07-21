#ifndef TYPES_H
#define TYPES_H
#include <uARMtypes.h>

typedef int devhdl_t(devreg_t*); /* Type of a function that takes a device register pointer as argument and
                                           returns int, for device handler routines */

typedef unsigned int cpu_t; /* cpu time type */

typedef enum {PSEUDOCLOCK, TIMESLICE, AGING} timcause_t; /* cause of interval timer interrupt type */

/* syscall labels */
typedef enum {CREATEPROCESS,TERMINATEPROCESS,SEMP,SEMV,SPECHDL,GETTIME,WAITCLOCK,IODEVOP,GETPIDS,WAITCHLD} syscall_t;

/* trap handler labels */
typedef enum {SPECPGMT,SPECTLB,SPECSYSBP} traphdl_t;

typedef unsigned int memaddr;

#endif // TYPES_H
