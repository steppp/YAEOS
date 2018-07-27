#include <uARMconst.h>
#include <uARMtypes.h>
#include <arch.h>
#include <libuarm.h>
#include <const.h>


#define BYTELEN 8

#define QPAGE 1024              /* size of a stack block. Each process is given one */
#define QPAGE_1 (QPAGE - 1)

#define CLOCKINTERVAL   100000UL        /* interval to V clock semaphore */
#define CLOCKLOOP               10
#define MINCLOCKLOOP            3000

#include <pcb.h>
#include <asl.h>

int term_mutex = 1;
int p1sem = 0;
int p1ok = 0;
int synp3 = 0;
int blkp3 = 1;
int endp3 = 0;
int testp5 = 1;
int stack_mutex = 1;
int p5p6_mutex = 1;
int p5p6race = 0;
int p7ok = 0;
int p7lock = 0;

memaddr next_stack;
state_t p1state;
state_t p2state;
state_t p3state;
state_t p4state;
state_t p5state;
state_t p6state;
state_t p7state;
void *p1p1addr, *p1p0addr;

int p3inc;

void print(char *msg) {
	unsigned int status;

	SYSCALL(SEMP, (memaddr)&term_mutex, 0, 0);

	while (*msg != '\0') {
		unsigned int command = DEV_TTRS_C_TRSMCHAR | (*msg << BYTELEN);
		status = SYSCALL(IODEVOP, command, DEV_REG_ADDR(IL_TERMINAL, 0) + 3 * WS,0);
		if ((status & DEV_TERM_STATUS) != DEV_TTRS_S_CHARTRSM)
        {
			PANIC();
        }
		if (((status >> BYTELEN) & DEV_TERM_STATUS) != *msg)
			PANIC();
		msg++;
	}
	SYSCALL(SEMV, (memaddr)&term_mutex, 0, 0);
}

memaddr get_stack_area(void) {
	memaddr ret_value;
	SYSCALL(SEMP, (memaddr)&stack_mutex, 0, 0);
	ret_value = next_stack;
	next_stack -= QPAGE;
	SYSCALL(SEMV, (memaddr)&stack_mutex, 0, 0);
	return ret_value;
}


void p1(void) {
	SYSCALL(GETPIDS, (memaddr)&p1p1addr, (memaddr)&p1p0addr, 0);
	SYSCALL(SEMV, (memaddr)&p1sem, 0, 0);
	SYSCALL(SEMP, (memaddr)&p1sem, 0, 0);

	SYSCALL(SEMV, (memaddr)&p1ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p1sem, 0, 0);
	print("P1 survived a terminate process syscall from p0\n");
	PANIC();
}

void p1a(void) {
	SYSCALL(SEMV, (memaddr)&p1ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p1sem, 0, 0);
	print("p1: Test of interleaved prints\n");
	SYSCALL(TERMINATEPROCESS, (memaddr) NULL, 0, 0);
	print("P1 survived a terminate process syscall\n");
	PANIC();
}

void p2(void) {
    int i;
	cpu_t time1, time2;
	cpu_t usr_t1, kernel_t1, wall_t1;
	cpu_t usr_t2, kernel_t2, wall_t2, glob_t2;
	while ((time2 - time1) < (CLOCKINTERVAL >> 1) )  {
		time1 = getTODLO();   /* time of day     */
		SYSCALL(WAITCLOCK, 0, 0, 0);
		time2 = getTODLO();     /* new time of day */
	}
	print("p2: WaitBlock Okay\n");

	SYSCALL(GETTIME, (memaddr)&usr_t1, (memaddr)&kernel_t1, (memaddr)&wall_t1);
	for (i = 0; i < CLOCKLOOP; i++)
		SYSCALL(WAITCLOCK, 0, 0, 0);
	SYSCALL(GETTIME, (memaddr)&usr_t2, (memaddr)&kernel_t2, (memaddr)&wall_t2);

	//SYSCALL(GETTIME, (int)&glob_t2, (int)&usr_t2, 0);    /* process time */

	if (((usr_t2 - usr_t1) > MINCLOCKLOOP) || ((usr_t2 + kernel_t2 - usr_t1 - kernel_t1) < MINCLOCKLOOP) ||
			(usr_t2 + kernel_t2 - usr_t1 - kernel_t1) > (wall_t2 - wall_t1))
		print("error: p2 - CPU time incorrectly maintained\n");
	else
		print("p2 - CPU time correctly maintained\n");

	SYSCALL(TERMINATEPROCESS, 0, 0, 0);
	print("P2 survived a terminate process syscall\n");
	PANIC();
}

void p3(void) {
	switch (p3inc++) {
		case 0:
			print("p3: first incarnation\n");
			break;
		case 1:
			print("p3: second incarnation\n");
			break;
	}
	SYSCALL(SEMV, (memaddr)&synp3, 0, 0);
	SYSCALL(SEMP, (memaddr)&blkp3, 0, 0);
	//SYSCALL(SEMP, (memaddr)&synp3, 0, 0);

	if (p3inc > 2) {
		print("error: second incarnation of p3 did not terminate\n");
		PANIC();
	}
	SYSCALL(SEMP, (memaddr)&synp3, 0, 0);
	p3state.sp = get_stack_area();
	SYSCALL(CREATEPROCESS, (memaddr)&p3state, 5, 0);

	SYSCALL(SEMP, (memaddr)&synp3, 0, 0);

	//SYSCALL(SEMP, (memaddr)&endp3, 0, 0);
	SYSCALL(TERMINATEPROCESS, 0, 0, 0);
	print("P3 survived a terminate process syscall\n");
	PANIC();
}

state_t p4pgm_new, p4tlb_new, p4sys_new;
state_t p4pgm_old, p4tlb_old, p4sys_old;

void p4a(void);
void p4b(void);
void p4pgm(void) {
	unsigned int exeCode = p4pgm_old.CP15_Cause;
	exeCode = CAUSE_EXCCODE_GET(exeCode);
	switch (exeCode) {
		case EXC_BUSINVFETCH:
			print("pgmTrapHandler - Access non-existent memory\n");
			p4pgm_old.pc = (memaddr)p4a;   /* Continue with p4a() */
			break;
		case EXC_RESERVEDINSTR:
			print("pgmTrapHandler - privileged instruction - set kernel mode on\n");
			print("p4 - try to call sys13 in kernel mode\n");
			/* return in kernel mode */
			p4pgm_old.cpsr = p4pgm_old.cpsr | 0xF;
			p4pgm_old.pc = (memaddr)p4b;
			break;

		case EXC_ADDRINVLOAD:
			print("pgmTrapHandler - Address Error: KSegOS w/KU=1\n");
			/* return in kernel mode */
			p4pgm_old.cpsr = p4pgm_old.cpsr | 0xF;
			p4pgm_old.pc = (memaddr)p4b;
			break;

		default:
			print("pgmTrapHandler - other program trap\n");
			PANIC();
			break;
	}
	LDST(&p4pgm_old);  /* "return" to old area (that changed meanwhile) */
}

void p4tlb(void) {
	print("memory management (tlb) trap - set user mode on\n");
	p4tlb_old.cpsr = p4tlb_old.cpsr & 0xFFFFFFF0;  /* user mode on */
	p4tlb_old.CP15_Control &= ~(0x1); /* disable VM */
	p4tlb_old.pc = (memaddr)p4b;  /* return to p4b */

	/* this should be done by p4b(). print() is called here because p4b() is executed in user mode */
	print("p4 - try to call sys13 in user mode\n");

	LDST(&p4tlb_old);
}

void p4sys(void) {
	unsigned int p4status = p4sys_old.cpsr;
	p4status = p4status & 0xF;
	if(p4status){
		print("High level SYS call from kernel mode process\n");
	} else {
		print("High level SYS call from user mode process\n");
		print("p4 - try to execute P() in user mode\n");
	}
	LDST(&p4sys_old);
}

void p4a(void) {
	unsigned int control;

	print("p4a trying to generate a tlb trap\n");
	control = getCONTROL();
	control = control | 0x00000001;
	setCONTROL(control);

	print("error - p4a still executing\n");
	PANIC();
}

void p4b(void) {

	SYSCALL(13, 0, 0, 0);

	SYSCALL(SEMP, (memaddr)&testp5, 0, 0);

	/* second call for SPECTRAPVEC/SPECPGMT terminate! */
	SYSCALL(SPECHDL, SPECPGMT, (int)&p4pgm_old, (int)&p4pgm_new);

	print("error - p4b still executing\n");
	PANIC();
}

void p4(void) {
	static memaddr *doesnotexist = (memaddr *) 0x34;
	print("p4 starts\n");
	STST(&p4pgm_new);
	p4pgm_new.sp = get_stack_area();
	p4pgm_new.pc = (memaddr) p4pgm;
	STST(&p4tlb_new);
	p4tlb_new.sp = get_stack_area();
	p4tlb_new.pc = (memaddr) p4tlb;
	STST(&p4sys_new);
	p4sys_new.sp = get_stack_area();
	p4sys_new.pc = (memaddr) p4sys;

	SYSCALL(SPECHDL, SPECPGMT, (int)&p4pgm_old, (int)&p4pgm_new);
	SYSCALL(SPECHDL, SPECTLB, (int)&p4tlb_old, (int)&p4tlb_new);
	SYSCALL(SPECHDL, SPECSYSBP, (int)&p4sys_old, (int)&p4sys_new);

	print("p4a trying to access non existent memory\n");
	*doesnotexist = *doesnotexist + 1;

	print("error - p4 still executing\n");
	PANIC();
}

void p5(void) {
	int i;
	for (i = 0; i < 10000; i++)
		;
	SYSCALL(SEMP, (memaddr)&p5p6_mutex, 0, 0);
	p5p6race = 5;
	SYSCALL(SEMV, (memaddr)&p5p6_mutex, 0, 0);

	print("p5 trying to request SYS13 without setting trap vector\n");

	SYSCALL(13, 0, 0, 0);

	print("error - p5 still executing\n");
	PANIC();
}

void p6(void) {
	int i;
	int j = 0;
	for (i = 0; i < 10000; i++)
		;
	SYSCALL(SEMP, (memaddr)&p5p6_mutex, 0, 0);
	p5p6race = 6;
	SYSCALL(SEMV, (memaddr)&p5p6_mutex, 0, 0);

	print("p6 generating a program trap without setting trap vector\n");

    // i /= j;
	i = *((int*)0x43);

	print("error - p6 still executing\n");
	PANIC();
}


void p7gc(void) {
	SYSCALL(SEMV, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7lock, 0, 0);
}

void p7c(void) {
	state_t p7gcstate;
	STST(&p7gcstate);
	p7gcstate.pc = (memaddr)p7gc;
	p7gcstate.sp = get_stack_area();
	SYSCALL(CREATEPROCESS, (memaddr)&p7gcstate, 5, 0);
	p7gcstate.sp = get_stack_area();
	SYSCALL(CREATEPROCESS, (memaddr)&p7gcstate, 5, 0);
	SYSCALL(SEMV, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7lock, 0, 0);
}

void p7(void) {
	state_t p7cstate;
	STST(&p7cstate);
	p7cstate.pc = (memaddr)p7c;
	p7cstate.sp = get_stack_area();
	SYSCALL(CREATEPROCESS, (memaddr)&p7cstate, 5, 0);
	p7cstate.sp = get_stack_area();
	SYSCALL(CREATEPROCESS, (memaddr)&p7cstate, 5, 0);
	SYSCALL(SEMP, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7ok, 0, 0);
	SYSCALL(SEMP, (memaddr)&p7ok, 0, 0);

	SYSCALL(TERMINATEPROCESS, 0, 0, 0);
}

void test(void) {
	void *p1addr, *p0addr, *p0paddr;
	STST(&p1state);
	next_stack = (p1state.sp + QPAGE_1) & ~QPAGE_1;
	p1state.sp = get_stack_area();
	p1state.pc = (memaddr) p1;

	STST(&p2state);
	p2state.sp = get_stack_area();
	p2state.pc = (memaddr) p2;

	STST(&p3state);
	p3state.sp = get_stack_area();
	p3state.pc = (memaddr) p3;

	STST(&p4state);
	p4state.sp = get_stack_area();
	p4state.pc = (memaddr) p4;

	STST(&p5state);
	p5state.sp = get_stack_area();
	p5state.pc = (memaddr) p5;

	STST(&p6state);
	p6state.sp = get_stack_area();
	p6state.pc = (memaddr) p6;

	STST(&p7state);
	p7state.sp = get_stack_area();
	p7state.pc = (memaddr) p7;

	print("test started\n");
	SYSCALL(CREATEPROCESS, (memaddr)&p1state, 10, (memaddr)&p1addr);

	SYSCALL(GETPIDS, (memaddr)&p0addr, (memaddr)&p0paddr, 0);
	if (p0paddr != NULL) {
		print("GETPIDS: wrong ppid of root process\n");
		PANIC();
	}

	SYSCALL(SEMP, (memaddr)&p1ok, 0, 0);
	if (p1p0addr != p0addr) {
		print("GETPIDS: wrong ppid of p1 process\n");
		PANIC();
	}
	if (p1p1addr != p1addr) {
		print("GETPIDS: wrong pid of p1 process\n");
		PANIC();
	}

	SYSCALL(TERMINATEPROCESS, (memaddr)p1addr, 0, 0);
	SYSCALL(WAITCHLD, 0, 0, 0);
	p1state.pc = (memaddr) p1a;
	SYSCALL(CREATEPROCESS, (memaddr)&p1state, 10, (memaddr)&p1addr);
	SYSCALL(SEMP, (memaddr)&p1ok, 0, 0);
	SYSCALL(SEMV, (memaddr)&p1sem, 0, 0);
	print("p0: Test of interleaved prints\n");
	SYSCALL(WAITCHLD, 0, 0, 0);
	SYSCALL(CREATEPROCESS, (memaddr)&p2state, 10, (memaddr)NULL);

	SYSCALL(WAITCHLD, 0, 0, 0);
	print("p0: p2 ended\n");

	SYSCALL(CREATEPROCESS, (memaddr)&p3state, 1, (memaddr)NULL);
	SYSCALL(CREATEPROCESS, (memaddr)&p4state, 3, (memaddr)NULL);
	SYSCALL(CREATEPROCESS, (memaddr)&p5state, 5, (memaddr)NULL);
  	SYSCALL(CREATEPROCESS, (memaddr)&p6state, 7, (memaddr)NULL);
	SYSCALL(WAITCHLD, 0, 0, 0);
	SYSCALL(WAITCHLD, 0, 0, 0);
	SYSCALL(WAITCHLD, 0, 0, 0);
	SYSCALL(WAITCHLD, 0, 0, 0);

	if (p5p6race != 5) {
		print("Wrong management of process priority\n");
		PANIC();
	}

	SYSCALL(CREATEPROCESS, (memaddr)&p7state, 1, (memaddr)NULL);
	SYSCALL(WAITCHLD, 0, 0, 0);

	print("test terminated (not so much to do today)\n");
	HALT();
}
