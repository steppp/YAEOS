#include <scheduler.h>
#include <uARMtypes.h>
#include <uARMconst.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <tree.h>
#include <semaphore.h>
#include <pcbFree.h>
#include <main.h>
#include <types.h>

void P(int *semaddr)
{
    /*
        Retrieve the semaphore address from register a2
        Check if the value of the semaphore is >0
        If it is, decrease it and return
        Otherwise, take the running process' pcb and insert it in the semaphore queue and call a
        scheduler dispatch
    */
    (*semaddr)--;
    if(*semaddr < 0)
    {
        pcb_t *p;   /* holds the former running pcb pointer*/
        p = suspend();
        insertBlocked(semaddr,p);
        dispatch();
    }
}

void V(int *semaddr)
{
    (*semaddr)++;
    if(*semaddr <= 0)   /* This means that some process was blocked on this semaphore */
    {
        pcb_t *p;   /* holds the unblocked pcb */
        p = removeBlocked(semaddr);
        if (p != NULL)
        {
            insertProcQ(&readyQueue,p);
            readyPcbs++;
        }
    }
}

/* SYSCALL 1, Tries to allocate a new process, if succesful configures it with the parameters and returns 0, else returns -1*/
int createProcess(state_t *statep, int priority, void **cpid){
	pcb_t *newproc= allocPcb();
	if ( newproc != NULL ){
		newproc->p_parent=runningPcb;
		newproc->p_s=*statep;
		newproc->old_priority=newproc->p_priority=priority;
		*cpid=newproc;
        newproc->waitingOnIO = 0;
        newproc->waitingForChild = 0;
        activePcbs++;
        insertProcQ(&readyQueue,newproc);
        readyPcbs++;
        newproc->usertime = newproc->kerneltime = newproc->wallclocktime = 0;
        newproc->sysbk_new = newproc->sysbk_old = newproc->tlb_new = newproc->tlb_old =
            newproc->pgmtrap_new = newproc->pgmtrap_old = NULL;
		return 0;
	}
	else{
		//You can't create a new Process, return -1
		return -1;
	}
}

/* Helping function for SYSCALL2, it recursievly kills all the children of a process*/
void killProcessSubtree(pcb_t *pcb){
	if(pcb == runningPcb) runningPcb = NULL;
	pcb_t *child;
	while ((child=removeChild(pcb)) !=NULL) killProcessSubtree(child);

    // resume the parent process if it has been suspended using the SYS10
    if (pcb->p_parent->waitingForChild) {       // if the parent is waiting for a child to terminate
        V(pcb->p_parent->p_semKey);             // unblock the parent process
        pcb->p_parent->waitingForChild = 0;     // the parent is no longer waiting for a child to terminate
    }

    if (outProcQ(&readyQueue,pcb) != NULL) /* removing it from ready queue*/
        readyPcbs--;
    if (pcb->waitingOnIO)   /* Process was blocked on I/O */
        softBlockedPcbs--;
    else
    {
        if (pcb->p_semKey != NULL) /* Process was blocked on a nondevice semaphore */
            (*(pcb->p_semKey))++;       /* The value of the semaphore needs to be adjusted*/
        activePcbs--;
    }

    outChildBlocked(pcb); /* removing the pcb from any queue it's blocked on */
    outChild(pcb);  /* Orphaning pcb */
	freePcb(pcb);
}

/* SYCALL 2 , kills the process and all its childs
 * What to do if it kills the running Process? It calls dispatch
 * How to check if it kills the running Process? It uses the global variable runningPcb , setting it to NULL if it kills it
 */
int terminateProcess(void * pid){
	if(pid==NULL) pid=runningPcb;
	killProcessSubtree(pid);
	if (runningPcb==NULL) dispatch();
	return 0;
}

/* SYSCALL 5, specifies what handlers to use, depending by trap.
 * When called a trap, the state of the calling process will copied in the 'old' area
 * and will loaded the state in the 'new' area.
 * Can be set only once a time for a process
 * Types can be:
    - 0 (SYSCALL/breakpoint)
    - 1 (TLB trap)
    - 2 (Program trap)
 * Returns 0 in case of success, -1 in case of failure.
*/
int specifyTrapHandler(int type, state_t *old, state_t *new) {
  switch (type) {
    case 0:
        //if currentProcess' areas are clean
        if (runningPcb->sysbk_new == NULL && runningPcb->sysbk_old == NULL) {
            //set areas
            runningPcb->sysbk_new = new;
            runningPcb->sysbk_old = old;
        }
        //else if are not clean are already set, failure! Return -1
        else {
            return -1;
        }
        break;

    case 1:
        //if currentProcess' areas are clean
        if (runningPcb->tlb_new == NULL && runningPcb->tlb_old == NULL) {
            //set areas
            runningPcb->tlb_new = new;
            runningPcb->tlb_old = old;
        }
        //else if are not clean are already set, failure! Return -1
        else {
            return -1;
        }
        break;

    case 2:
        //if currentProcess' areas are clean
        if (runningPcb->pgmtrap_new == NULL && runningPcb->pgmtrap_old == NULL) {
            //set areas
            runningPcb->pgmtrap_new = new;
            runningPcb->pgmtrap_old = old;
        }
        //else if are not clean are already set, failure! Return -1
        else {
            return -1;
        }
        break;

    default:
      return -1;
  }
  return 0; //Success!
}

/* SYSCALL 6, returns times of current running processes
 * user contains the time spent in user mode of the process
 * kernel contains the time spent in kernel mode of the process
 * wallclock contains the time from first process load.
 */

void getTimes(cpu_t *user, cpu_t *kernel, cpu_t *wallclock) { 
#if 0
  user = &(runningPcb->usertime);
  kernel = &(runningPcb->kerneltime);
  wallclock = &(runningPcb->wallclocktime);
/* Andrea: The variables are passed by address to contain the return values */
#endif 
  *user = runningPcb->usertime;
  *kernel = runningPcb->kerneltime;
  *wallclock = runningPcb->wallclocktime;
}

/* SYSCALL 7: Stops the current running process and adds it to the pseudoClockSem*/

void waitForClock(){

    P(&pseudoClockSem); /* If the function is this short, we could include this line directly into
                           the Syshandler */
}

/*  Helper function for syscall 8 , given a register it calculates both its interrupt Line and its device number
 *  If the interrupt Line is a terminal, it calculates if its a Recv or a Trasm command
 *  the intLine, devNo and termIO parameters are used to return values
 */

void getDeviceFromRegister(int * intLine , int * devNo, int * termIO, unsigned int *comm_device_register){
    /* Calculates the base dtpreg_t from the formula  "comm_device_register=devreg+sizeof(unsigned it)" */
    unsigned int devAddrCalculated= (memaddr)comm_device_register - WS;
    /* Given the addres of the devreg we can calculate both the intline and the devNo from the formula "devAddrBase = 0x40 + ((Intline-3)*0x80)+(DevNo*0x10)" 
     * where baseIntLineSize will be the "0x80" and baseOffset the "0x40", these are both declared to improve readability 
     */
    unsigned int baseIntLineSize=8*DEV_REG_SIZE;
    unsigned int baseOffset=DEV_REG_START;
    *intLine= ((devAddrCalculated-baseOffset)/baseIntLineSize)+INT_LOWEST;
    *devNo= ((devAddrCalculated-baseOffset)%baseIntLineSize) / DEV_REG_SIZE;
    *termIO= -1; //Dummy value assigned just to not have a pointer to NULL in case intLine is not 7
    if (*intLine==7){
    /* if the Interrupt Line is 7, the register belongs to a Terminal, we need to check if its input or output
     *      if its input(RECV) the devAddrCalculated should be equal to its actual address, so we check it against the formula "devAddrBase=0x40+((Intline-3)*0x80)+(DevNo*0x10)" and set the termIO accordingly
     *      else its output(TRANSM) and we set the termIO accordingly
     */
        if (devAddrCalculated ==
                (baseOffset+((*intLine - INT_LOWEST)*baseIntLineSize)+((*devNo)*DEV_REG_SIZE))){
            *termIO=RECV;
        }
        else *termIO=TRANSM;
    }
}

/* SYSCALL 8: 
 *  Activates the I/O operations copying the command in the appropriate command device register
 *  the caller will be suspended and appended to the appropiate semaphore ( using a normalDevice semaphore if the interupt Line is <7 or a terminal semaphone if =7)
 */

unsigned int ioOperation(unsigned int command, unsigned int *comm_device_register){

    int intLine,devNo,termIO;
    getDeviceFromRegister(&intLine ,&devNo, &termIO, comm_device_register);
    if (intLine<7){
        P(&normalDevices[intLine - INT_LOWEST][devNo]);
        *comm_device_register=command;
    }
    else if(intLine==7){
        P(&terminals[devNo][termIO]);
        *comm_device_register=command;
    }
    return 0;
}


/*
 * SYSCALL 9
 * Returns this process' PID and its father's one
 * If pid or ppid is NULL, the associated PID is not returned
 * Returns NULL as the ppid if this is the root process
 */
void getPids(void **pid, void **ppid) {
    if (ppid != NULL) {
        if (runningPcb->p_parent != NULL)
            *ppid = runningPcb->p_parent;
        else *ppid = NULL;
    }

    if (pid != NULL)
        *pid = runningPcb;
}

/*
 * SYSCALL 10
 * Puts the process in a suspended state waiting for a child to finish its execution
 */
void waitChild() {
    int c_sem = 1;  // verificare che questa dichiarazione non possa verificare casi di dangling references
    runningPcb->waitingForChild = 1;    /* This will be used later to tell wether this process were waiting for a child to end  */
    P(&c_sem);                          /* Suspend the process */

    /* perform extra work when the process resumes  */
}

void pgmTrapHandler(){
    /* 
     *   Checks if a SYS5 has been called for the current process (The one who triggered the pgmTrap)
     *       If it is defined, copies the content of the OLDAREA into the processes oldarea and loads the new area
     *      If its not , calls SYS2 to abort the process
     */
     
    if (runningPcb->pgmtrap_new !=NULL){
        *(runningPcb->pgmtrap_old)=  *((state_t*)PGMTRAP_OLDAREA);
        LDST(runningPcb->pgmtrap_new);          
    }
    else terminateProcess();     
}

void tlbHandler(){
    /* 
     *   Checks if a SYS5 has been called for the current process (The one who triggered the tlb)
     *       If it is defined, copies the content of the OLDAREA into the processes oldarea and loads the new area
     *      If its not , calls SYS2 to abort the process
     */
     
    if (runningPcb->tlb_new !=NULL){
        *(runningPcb->tlb_old)=  *((state_t*)TLB_OLDAREA);
        LDST(runningPcb->tlb_new);          
    }
    else terminateProcess();     
}

void sysHandler(){
    /*
        * Gets the newarea and checks the cause of the exception
        *   If Breakpoint:
        *       Passes it up to the higer level hander if present, if not declared terminates the pocess
        *   If SYSCALL:
        *       Checks if the process is running in system(kernel) mode, if positive            
        *           Gets the syscall number stored in the a1 register of SYSBK_NEWAREA
        *           According to the number it calls the appropriate function.
        *           If the value is >10 sends it to the corresponding higher level handler, if there isnt one terminates the process
        *       If not the process is running in user mode!
        *           Checks if its trying to use a reserved syscall <=10
        *               If yes, send it to the   PgmTrap handler with the error code EXC_RESERVEDINSTR
        *               if not, sends it to the corresponding higher level handler, if there isnt one terminates the process
    */

    state_t *userRegisters = (state_t*) SYSBK_OLDAREA;
    /* Checks the cause */
    if(CAUSE_EXCCODE_GET(userRegisters->CP15_Cause) == EXC_BREAKPOINT){
        /*Breakpoint, sends it to the higher level handler if present, if not terminates the process*/
     
        if (runningPcb->sysbk_new !=NULL){
            *(runningPcb->sysbk_old)=  *((state_t*)SYSBK_OLDAREA);
            LDST(runningPcb->sysbk_new);          
        }
        else terminateProcess(); 
    }

    else if(CAUSE_EXCCODE_GET(userRegisters->CP15_Cause) == EXC_SYSCALL){
        /*  SYScall
         *  Checks if the process is running in system mode 
         */
        if (userRegisters->cpsr==STATUS_SYS_MODE){
            /* If yes it handles the syscall selecting which one to call and passing the correct parameters */
            int succesful=5; /* Helper integer that will store if the syscall has ended correctly for those who return something, initialized to an impossible value so the checks cant uncorrectly pass*/
            switch(userRegisters->a1){
                case CREATEPROCESS:
                    /*  a2 should contain the physical address of a processor state area at the time this instruction is executed and a3 should contain the priority level
                     *  if it returns error it means that you cant allocate more processes, puts -1 in the return register
                     */
                    //TODO: Visto che tanto cpid è il parametro che devo mettere in a1, e non viene controllato solo scritto, glielo passo direttamente e ci fa quello che vuole, togliere il commento se è giusto , altrimenti bisogna un attimo aggiustarlo
                    succesful=createProcess((state_t *)userRegisters->a2 , (int) userRegisters->a3,(void **) userRegisters->a1);
                    if (succesful==-1) userRegisters->a1=-1;
                    break;
                case TERMINATEPROCESS:
                    /* a2 should contain the value of the designated process’ PID */
                    succesful=terminateProcess( (void *)userRegisters->a2 );
                    if (succesful!=0){
                            tprint("Syscall 2 (Terminate Process) returned an error state, literally impossible (MESSAGE BY KERNEL, NOT P2TEST)");
                            PANIC();
                    }
                    break;
                case SEMP:
                    /* a2 should contain the physical address of the semaphore to be V’ed */
                    P((int*) userRegisters->a2);
                    break;
                case SEMV:
                    /* a2 should contain the physical address of the semaphore to be P’ed */
                    V((int*)userRegisters->a2);
                    break;
                case SPECHDL:
                    /*
                     *  a2 should contain which handler it will modify, a3 should contain a pointer to the old state, and a4 a pointer to the new state.
                     *  if the syscall returns error means that SYS5 was called more than once on the process, its not allowed so it should be treatead as a SYS2 instead
                     */
                    succesful=specifyTrapHandler((int)userRegisters->a2, (state_t *)userRegisters->a3, (state_t *)userRegisters->a4);
#if 0
                    if (succesful==-1) terminateProcess(runningPcb);
                    /* Andrea: minor error, 1 argument missing Igor: Redundant, if terminateProcess doesent get any argument it will default to the runningPbc, I think it would be more elegant this way*/
#endif
                    if (succesful==-1) terminateProcess();
                    break;
                case GETTIME:
                    // Retrieves and returns the cpu times putting them in the appropriate return registers */
                    getTimes((cpu_t *)userRegisters->a1, (cpu_t *)userRegisters->a2, (cpu_t *)userRegisters->a3);
                    break;
                case WAITCLOCK:
                    /* No arguments necessary, it just calls the appropriate function */
                    waitForClock();
                    break;
                case IODEVOP:
                    /* a2 should contain the command and a3 should contain the device_command_register the command needs to be put into */
                    succesful=ioOperation((unsigned int) userRegisters->a2, (unsigned int *)userRegisters->a3);
                    userRegisters->a1=runningPcb->p_s.a1; //TODO: E' Giusto? in a1 mi serve lo status, e nell'interrupt handler me lo mette nel suo p_s.a1
                    if (succesful!=0){
                        tprint("Syscall 8 (IO operation) returned an error state, literally impossible (MESSAGE BY KERNEL, NOT P2TEST)");
                        PANIC();
                    }
                    break;
                case GETPIDS:
                    /* TODO: Controllare se è corretto, molto probabilmente non lo è visto che le specifiche di Davoli e della tesi sono diverse
                        Assumo che i valori da passare per parametro sono in a2 e a3, perchè comunque controlla che non siano null
                        e li restituisco inserendoli in a1 e a2 come nelle altre syscall.
                        Se ho fatto una cazzata scrivete nel gruppo "igor non programmare dopo che hai bevuto" e capirò
                    */
                    getPids((void **)userRegisters->a2, (void **)userRegisters->a3);
                    // Shifts the returned values so the first one is in a1.
                    userRegisters->a1=userRegisters->a2;
                    userRegisters->a2=userRegisters->a3;            
                    break;
                case WAITCHLD:
                    // No arguments necessary, it just calls the appropriate function */
                    waitChild();
                    break;
                default:
                    /* 
                        * Syscall >10.
                        *   Checks if a SYS5 has been called for the current process (The one who triggered the SYSCALL)
                        *       If it is defined, copies the content of the OLDAREA into the processes oldarea and loads the new area
                        *       If its not , calls SYS2 to abort the process
                    */

                    if (runningPcb->sysbk_new !=NULL){
                        *(runningPcb->sysbk_old)=  *((state_t*)SYSBK_OLDAREA);
                        LDST(runningPcb->sysbk_new);          
                    }
                    else terminateProcess(); 
                    break;
            }
            /*
            Andrea: missing, if a syscall return it's a duty of the OS to restore the
            processor's state to the one of the process that called the syscall, eventually with the
            a1 register modified to keep the return value if needed
             */
            LDST(userRegisters); 
        }
        else{
            /*  It was not running in kernel mode
             *  Checks if its trying to run a reserved syscall (<=10)
             *      If yes, send it to the   PgmTrap handler with the error code EXC_RESERVEDINSTR
             *      if not, sends it to the corresponding higher level handler, if there isnt one terminates the process
             */
            if(userRegisters->a1 <= 10){
                runningPcb->CP15_Cause=EXC_RESERVEDINSTR;
                pgmTrapHandler();
            }
            else{
                if (runningPcb->sysbk_new !=NULL){
                    *(runningPcb->sysbk_old)=  *((state_t*)SYSBK_OLDAREA);
                    LDST(runningPcb->sysbk_new);          
                }
                else terminateProcess();
            }
            
        }
    }
}
