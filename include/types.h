#ifndef TYPES_H
#define TYPES_H
#include <uARMtypes.h>

/* Cpu time type */
typedef unsigned long long cpu_t; 

/* Cause of interval timer interrupt type */
typedef enum {PSEUDOCLOCK, TIMESLICE, AGING} timcause_t; 

/* Syscall labels */
enum {CREATEPROCESS = 1,TERMINATEPROCESS = 2,SEMP = 3,SEMV = 4,SPECHDL = 5,GETTIME = 6,WAITCLOCK =
    7,IODEVOP = 8,GETPIDS = 9,WAITCHLD = 10};

/* Trap handler labels */
enum {SPECPGMT,SPECTLB,SPECSYSBP};

typedef unsigned int memaddr;

#endif // TYPES_H
