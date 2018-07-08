#include <scheduler.h>
#include <uARMconst.h>
#include <uARMtypes.h>
#include <libuarm.h>
#include <pcb.h>
#include <list.h>
#include <main.h>
#include <interrupts.h>
#include <const.h>



void pgmTrapHandler(){
    /* 
     *   Checks if a SYS5 has been called for the current process (The one who triggered the pgmTrap)
     *       If it is defined, saves the current status in the old area and loads the new area in the current status of the process
     *      If its not , calls SYS2 to abort the process
     */
     //TODO: In caso di riattivazione a seguito della gestione di un Trap (e.g. Page Fault), l’istruzione che ha causato il trap va ripetuta.
     // in Register 6 è inserito l'indirizzo della memoria che ha causato l'eccezione Page Fault , però non trovo ne il numero associato al PageFault, ne sono sicuro che è una cosa che dobbiamo fare noi
        
    if (runningPcb->pgmtrap_new !=NULL){
        runningPcb->pgmtrap_old=  &(runningPcb->p_s);
        runningPcb->p_s=*(runningPcb->pgmtrap_new);          
    }
    else terminateProcess();        
}

void tlbHandler(){
    /* 
     *   Checks if a SYS5 has been called for the current process (The one who triggered the tlb)
     *       If it is defined, saves the current status in the old area and loads the new area in the current status of the process
     *      If its not , calls SYS2 to abort the process
     */
    if (runningPcb->tlb_new !=NULL){
        runningPcb->tlb_old=  &(runningPcb->p_s);
        runningPcb->p_s=*(runningPcb->tlb_new);          
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

    state_t *userRegisters = (state_t*) SYSBK_NEWAREA;
    // Checks the cause
    if(userRegisters->CP15_Cause==9){
        //Breakpoint, sends it to the higher level handler if present, if not terminates the process
        if (runningPcb->sysbk_new !=NULL){
            runningPcb->sysbk_old=  &(runningPcb->p_s);
            runningPcb->p_s=*(runningPcb->sysbk_new);          
        }
        else terminateProcess();
            
    }

    else if(userRegisters->CP15_Cause==8){
        /*  SYScall
         *  Checks if the process is running in system mode 
         */
        if (userRegisters->cpsr==STATUS_SYS_MODE){
            //If yes it sends it handles the syscall selecting which one to call and passing the correct parameters */
            int succesful=0; // Helper integer that will store if the syscall has ended correctly for those who return something
            switch(userRegisters->a1){
                case 1:
                    /*  a2 should contain the physical address of a processor state area at the time this instruction is executed and a3 should contain the priority level
                     *  if it returns error it means that you cant allocate more processes, puts -1 in the return register
                     */
                    //TODO: Visto che tanto cpid è il parametro che devo mettere in a1, e non viene controllato solo scritto, glielo passo direttamente e ci fa quello che vuole, togliere il commento se è giusto , altrimenti bisogna un attimo aggiustarlo
                    succesful=createProcess((state_t *)userRegisters->a2 , (int) userRegisters->a3,(void **) userRegisters->a1);
                    if (succesful==-1) userRegisters->a1=-1;
                    break;
                case 2:
                    /* a2 should contain the value of the designated process’ PID */
                    succesful=terminateProcess( (void *)userRegisters->a2 );
                    if (succesful!=0){
                            tprint("Syscall 2 (Terminate Process) returned an error state, literally impossible (MESSAGE BY KERNEL, NOT P2TEST)");
                            PANIC();
                    }
                    break;
                case 3:
                    /* a2 should contain the physical address of the semaphore to be V’ed */
                    V((int*) userRegisters->a2);
                    break;
                case 4:
                    /* a2 should contain the physical address of the semaphore to be P’ed */
                    P((int*)userRegisters->a2);
                    break;
                case 5:
                    /*
                     *  a2 should contain which handler it will modify, a3 should contain a pointer to the old state, and a4 a pointer to the new state.
                     *  if the syscall returns error means that SYS5 was called more than once on the process, its not allowed so it should be treatead as a SYS2 instead
                     */
                    succesful=specifyTrapHandler((int)userRegisters->a2, (state_t *)userRegisters->a3, (state_t *)userRegisters->a4);
                    if (succesful==-1) terminateProcess();
                    break;
                case 6:
                    // Retrieves and returns the cpu times putting them in the appropriate return registers */
                    getTimes((cputtime_t *)userRegisters->a1, (cputtime_t *)userRegisters->a2, (cputtime_t *)userRegisters->a3);
                    break;
                case 7:
                    // No arguments necessary, it just calls the appropriate function */
                    waitForClock();
                    break;
                case 8:
                    //TODO: Aggiungere quando creata
                    break;
                case 9:
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
                case 10:
                    // No arguments necessary, it just calls the appropriate function */
                    waitChild();
                    break;
                default:
                    /* 
                        * Syscall >10.
                        * Checks if a SYS5 has been called for the current process (The one who triggered the SYSCALL)
                        *   If it is defined, saves the current status in the old area and loads the new area in the current status of the process
                        +   If its not , calls SYS2 to abort the process
                    */
                    if (runningPcb->sysbk_new !=NULL){

                        runningPcb->sysbk_old=  &(runningPcb->p_s);
                        runningPcb->p_s=*(runningPcb->sysbk_new);          
                    }
                    else terminateProcess();

                    break;
            }
        }
        else{
            /*  It was not running in kernel mode
             *  Checks if its trying to run a reserved syscall (<=10)
             *      If yes, send it to the   PgmTrap handler with the error code EXC_RESERVEDINSTR
             *      if not, sends it to the corresponding higher level handler, if there isnt one terminates the process
            */
            if(userRegisters->a1 <= 10){
                runningPcb->pgmtrap_old=runningPcb->sysbk_old;
                runningPcb->pgmtrap_old->CP15_Cause=EXC_RESERVEDINSTR;
                pgmTrapHandler();
            }
            else{
                if (runningPcb->sysbk_new !=NULL){
                    runningPcb->sysbk_old=  &(runningPcb->p_s);
                    runningPcb->p_s=*(runningPcb->sysbk_new);          
                }
                else terminateProcess();        
            }
            
        }
    }
}



void dispatch()
{
    /*
        Check if there are processes in the ready queue.
            If there are, check if there is a running process.
                If there is one, load CPU state in a temp variable and save a temp pointer to it.
                Load the first process in the ready queue on the cpu, removing it
            Else
                do nothing
   */
    if (readyPcbs > 0)
    {
        pcb_t *tmp = NULL;  /* temp var to store the running pcb*/
        if (runningPcb != NULL)
            tmp = suspend();
        runningPcb = removeProcQ(&readyQueue);
        readyPcbs--;
        if (tmp != NULL)
            insertProcQ(&readyQueue,tmp);
        updateTimer();  /* Load the new timer */
        LDST(&runningPcb->p_s); /* load the new PCB */
    }
}

pcb_t *suspend()
{
    if (runningPcb != NULL)
    {
        pcb_t *p;   /* holds the running pcb pointer for return*/
        runningPcb->p_s = *((state_t *) SYSBK_OLDAREA); /* save the old processor state in the process' pcb */
        runningPcb->p_priority = runningPcb->old_priority; /* restoring its old priority */
        p = runningPcb;
        runningPcb = NULL;
        return p;
    }
    else
        return NULL;
}

void increasePriority(pcb_t *p, void *arg)
{
    for (pcb_t *i = p; i != NULL; i = i->p_next)
        if (p->p_priority < MAXPRIO)
            i->p_priority++;
}
