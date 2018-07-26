# **DOCUMENTATION AND DESIGN CHOICES FOR YAEOS**

---

## PCB design 

The PCB contains the following fields:

>- **p_next**: Pointer for the next element of the list. 
>- **p_parent, p_first_child, p_sib**: Pointers for tree hierarchy of processes.
>- **p_s**: CPU state. Useful to save the state of a process to resume it at a later time.
>- **p_priority**: Process' priority. The scheduler can increase the priority according to aging policy, up
to the maximum limit of MAXPRIO.
>- **old_priority**: We keep track of the original priority of the process, i.e. the one the process
was created with. This can be restored at the appropriate time.
>- **p_semKey**: A pointer to the semaphore the process is blocked on (if any).
>- **waitingOnIo**: 1 if process is waiting for I/O, 0 otherwise.
>- **childSem**: Semaphore used to block the process when it's waiting for a 
child to terminate. This semaphore will be managed by the child.
>- **waitingForChild**: 1 if the process is waiting for a child , 0 otherwise. Useful to unblock a
parent that's waiting for a child to terminate.
>- **usertime**: Process' time spent in user mode
>- **kerneltime**: Process' time spent in kernel mode
>- **lasttime**: Last TOD marker. We use it for keep track of last time the process was started.
Useful to keep track of the process' user time
>- **wallclocktime**: Process' creation TOD. Useful for calculating the wallclocktime
>- **sysbk_new, sysbk_old, tlb_new, tlb_old, pgmtrap_new, pgmtrap_old**: 
Per-trap handlers

---

## Scheduler

The YAEOS scheduler is a priority-based scheduler with aging and a timeslice. The aging period is
10ms, whereas the duration of a timeslice is 3ms. Conceptually, the dispatcher is like a Singleton
Object with some "member fields" to keep the status of the system and some "member functions" to
interact with it. The "member fields" of the scheduler are the ready queue, a priority list for the
processes that are ready to be started, a pointer to the currently running process, the number of
processes in the ready queue, the number of active processes and the number of softblocked
processes. The so called "active processes" are all the processes that are not waiting on a device
semaphore (these include the ready processes), whereas the softblocked processes are the ones
waiting on a device semaphore. The "member functions" of the scheduler are the dispatch function,
the suspend function, the increasePriority function, the restoreRunningProcess function, the
insertInReady function, the three functions for processes' time accounting (userTimeAccounting,
kernelTimeAccounting and freezeLastTime) and the passup function. 

Parts of the OS call the methods of the scheduler they need to and handle the counters directly.
Some aspects of the of the loading of a new process, like the saving of the state of a process in
its pcb and some adjustments to the pc of a process, are done only in the scheduler module, so they
are abstracted by the scheduler methods. Still, there is not a single function that checks the state
of the system and takes the right scheduling decision.

There may be, at some point, no processes in the ready queue. This could be a normal situation, as
there could be some processes that are blocked on some device or on the pseudoclock, or there could
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
interrupt line according to their precedence order (as defined by the YAEOS specification).

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
or aging tick. If it's close enough or it has been passed then the next cause for the timer
interrupt will be set accordingly and the appropriate actions will be taken. These include the
increment of the tick counter.

The update of the timer is done as follows: first the deadline of the next pseudoclock tick is
calculated. If this is less than the timeslice duration then the cause of the next interval timer's
interrupt will be the pseudoclock tick. If the deadline has been passed the timer is not set: this
way its interrupt is not ackowledged and so it is handled again the moment the interrupts are
unmasked. Otherwise, the timer is set to trigger an interrupt on the next pseudoclock tick. The same
thing is done for the aging tick if the pseudoclock deadline is further away in the future than the
duration of a timeslice. If the aging tick is also too far in the future to worry about it then the
timer is set to the timeslice duration and the cause is set accordingly.

---

## Syscalls 

### SYS1: Create process
Tries to allocate a new pcb for a new process. If succesful it sets its fields appropriately and
returns 0, else returns -1.

Fields setting consists of setting times, hierarchy, priorities, trap handlers. 

In the end it inserts the new process in ready queue.

### SYS2: Terminate process

Terminates the given process and all its childs.

If the given process to terminate is NULL then the syscall terminates currently running process. 

If the process to terminate is the currently running process a scheduler dispatch will be necessary.

If the parent of a terminated process is waiting for a child to terminate then the parent is notified
with the child's termination and can continue with its execution.

Every blocked/suspended process in the subtree is removed from any queue is blocked on and updates
to the kernel counters are done, according to each terminated process' status.

It returns 0 if the process is correctly terminated, -1 otherwise.

### Note on the semaphores

The semaphores used in YAEOS are handled in such a way that their internal value can become
negative, with the absolute value of it being the number of processes waiting on the semaphore. This
proved to be very useful in some sections of the code, even though it doesn't respect the definition
of a semaphore, although its semantics are completely respected. The semaphore are simple semaphores.

### SYS3: P

This syscall decrements a given semaphore and blocks the currently running process on that
semaphore if its value goes below 0

### SYS4 : V

This syscall increments a given semaphore and, if the new value is less than or equal to 0, a
process that was blocked on it is unblocked and inserted in the ready queue.

### SYS5: Specify trap handlers

This syscall specifies which handler (Syscall/breackpoint, TLB trap or Program trap) should be
called when a trap is raised. 

This syscall should be called only once for a process.

The old area to save the process' state at the time of the trap and the new area to be loaded
in response to the trap need to be specified.

This syscall returns 0 in case of success, -1 otherwise.
