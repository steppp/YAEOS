#ifndef TYPES_H
#define TYPES_H
#include <uARMtypes.h>

typedef int devhdl_t(devreg_t*); /* Type of a function that takes a device register pointer as argument and
                                           returns int, for device handler routines */

typedef unsigned long long cpu_t; /* cpu time type */


typedef enum {PSEUDOCLOCK, TIMESLICE, AGING} timcause_t; /* cause of interval timer interrupt type */

/* Special cases when there are no more ready PCBs labels */
typedef enum{ STATUSDEADLOCK=1, STATUSWAIT=2, STATUSHALT=3} systemstatus_t;

/* syscall labels */
enum {CREATEPROCESS = 1,TERMINATEPROCESS = 2,SEMP = 3,SEMV = 4,SPECHDL = 5,GETTIME = 6,WAITCLOCK =
    7,IODEVOP = 8,GETPIDS = 9,WAITCHLD = 10};



/* trap handler labels */
enum {SPECPGMT,SPECTLB,SPECSYSBP};

typedef unsigned int memaddr;

#endif // TYPES_H
