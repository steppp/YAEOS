# **DOCUMENTATION AND DESIGN CHOICES FOR YAEOS PROJECT**

---

## PCB design 

Fields used in PCB:

>- **p_next**: Pointer for the next element of the list. 
>- **p_parent, p_first_child, p_sib**: Pointers for tree hierarchy of processes
>- **p_s**: State of CPU process fields.
>- **p_priority**: Process' priority. 
Scheduler can increase priority by ageing policy.
>- **old_priority**: We keep track of the birth's priority of the pcb. 
'p_priority' field contains the priority interactively modified by scheduler's 
ageing policy. When priority restore is needed, old_priority is used.
>- **p_semKey**: Process' semaphore pointer.
>- **waitingOnIo**: 1 if process is waigint for IO, 0 otherwise
>- **childSem**: Semaphore used for blocking process when is waiting for a 
child. This semaphore will be managed by the child.
>- **waitingForChild**: 1 if process is waiting for a child (so the child will 
unblock parent when it terminate), 0 otherwise.
>- **usertime**: Process' time spent in user mode
>- **kerneltime**: Process' time spent in kernel mode
>- **lasttime**: Last TOD marker. We use it for keep track of last
 activation/deactivation 
>- **wallclocktime**: Process' TOD when starting. Used for calculating total 
 wallclocktime
>- **sysbk_new, sysbk_old, tlb_new, tlb_old, pgmtrap_new, pgmtrap_old**: 
Per-trap handler

---

## Scheduler

The YAEOS scheduler is a priority-based scheduler with aging and a timeslice. The aging period is
10ms, whereas the duration of a timeslice is 3ms. Conceptually, the dispatcher is like a Singleton
Object with some "member fields" to keep the status of the system and some "member functions" to
interact with it. The "member fields" of the scheduler are the readyQueue, a pointer to the
currently running process, the number of processes in the ready queue, the number of active
processes and the number of softblocked processes. The so called active processes are all the
processes that are not waiting on a device semaphore (these include the ready processes), whereas
the softblocked processes are the ones waiting on a device semaphore. The "member functions" are the
dispatch function, the suspend function, the increasePriority function, the restoreRunningProcess
function, the insertInReady function, the three functions for processes' time accounting
(userTimeAccounting, kernelTimeAccounting and freeLastTime) and the passup function. Parts of the OS
call the methods of the scheduler they need to and handle the counters directly. Some aspects of the
of the loading of a new process, like the saving of the state of a process in its pcb and some adjustments
to the pc of a process, are done only in the scheduler module, so they are abstracted by the
scheduler methods. Still, there is not a single function that checks the state of the system and
takes the right scheduling decision.

It may happen that there are no processes in the ready queue. This could be a normal situation, as
there could be some processes that are blocked on some device or on the pseudoclock or there could
be no processes to execute at all, or there might be a deadlock. In the first case the dispatch
function puts the system in a waiting state, with all interrupts unmasked, waiting for an interrupt
from a device or from the timer to unblock a process. In the second case the system is shut
down. Finally in the third case a PANIC instruction is executed.

---

## Interrupts

Interrupts are handled by a function that, based on the device that caused the interrupt, takes the
appropriate handling decisions. In pretty much every case, except for the timer, the handler determines
the device that caused the interrupt, unblocks a process from the semaphore associated with it,
acknowledges the interrupt and saves the return status from the device in the a1 register of the
state of the pcb of the process that was waiting on the device. The terminal devices are slightly
different in that they are "double devices": they act like a pair of devices, one of which is the
receiver and the other is the transmitter. The determination of the device that caused the interrupt
is made by scanning each interrupt line and each bit in the bitmap associated with a particular
interrupt line according to their precedence (as defined by the YAEOS specification).

Interrupts are handled one at a time: when an interrupt is generated the interrupt handler routine
handles it and then gives back control to processes or, if there are no processes to give control
to, it goes in a waiting state to wait for an interrupt to wake up some process. In both cases the
interrupts are unmasked. If during the handling of the first interrupt others have been generated
then, as soon as the interrupts are unmasked, uarm gives control back to the interrupt handler
routine, that handles the new interrupt. This effectively creates a loop in which all interrupts are
handled before giving control back to processes.

The timer interrupts are handled based on the cause of the interrupt. This is maintained in a
separate variable that is properly set when the timer is updated. If the cause of the interrupt was
the pseudoclock tick then all the processes blocked on the pseudoclock semaphore are unblocked. If
the cause was the aging tick, then all processes (if any) in the ready queue have their priority
incremented. Otherwise a timeslice expired and the running process needs to be suspended and a
process dispatch needs to take place. This is handled by the scheduler.

The pseudoclock and aging clock ticks are handled in the same way: a global variable for each keeps
the count of ticks. Based on that it's possible to calculate the next deadline for the pseudoclock
or aging tick. If it's close enough or it has been passed than the next cause for the timer
interrupt will be set accordingly and the appropriate actions will be taken. These include the
increment of the tick counter.

The update of the timer is done as follows: first the deadline of the next pseudoclock tick is
calculated. If this is less than the timeslice duration than the cause of the next interval timer's
interrupt will be the pseudoclock tick. If the deadline has been passed the timer is not set: this
way its interrupt is not ackowledged and so it is handled again the moment the interrupts are
unmasked. Otherwise, the timer is set to trigger an interrupt on the next pseudoclock tick. The same
thing is done for the aging tick if the pseudoclock deadline is further away in the future than the
duration of a timeslice. If the aging tick is also too far in the future to worry about it then the
timer is set to the timeslice duration and the cause is set accordingly.

---

## Syscalls 

### SYS1: Create process
Tries to allocate a new process, if succesful configures it with the parameters 
and returns 0, else returns -1.

Parameters configurations consists in setting times, hierarchy, priorities, 
trap handlers. 

In the end puts the new process in ready queue.

### SYS2: Terminate process
Kills the process and all its childs.

If the process to kill passed is NULL then kills current running process. 

If the process to kill is the current running process the function calls the
scheduler dispatcher.

If in the subtree there is a parent processes waiting for child it unblocks the
 parent process.

Every blocked/suspended process in the subtree are removed from any queue is 
blocked on and updates kernel fields in according of each process' status.

It returns 0 if process is killed, -1 otherwise.

### Note on the semaphores

The semaphores used in YAEOS are handled in such a way that their internal value can become
negative, with the absolute value of it being the number of processes waiting on the semaphore. This
proved to be very useful in some sections of the code, even though it doesn't respect the definition
of a semaphore, although its semantics is completely respected. The semaphore are simple semaphores.

### SYS3: P
This makes a P in the semaphore passed by parameter. 

His behaviour is according to a simple semaphore.

### SYS4 : V

This makes a V in the semaphore passed by parameter. 

His behaviour is according to a simple semaphore.


### SYS5: Specify trap handlers

Save handlers specifying what type of handler (Syscall/breackpoint, TLB trap or
Program trap) should be called when a trap is raised. 

This syscall should be called only once a time for a process.

You have to specify old and new areas where are saved/loaded the process status
when specified trap has raised.

This syscall returns 0 in case of success, -1 otherwise.s
