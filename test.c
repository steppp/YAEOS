#include <libuarm.h>
#include <uARMconst.h>
#include <uARMtypes.h>
#include <arch.h>

int global = 1;

void test()
{
    tprint("It worked, my time machine worked!\n");
    global = 0;
}

int main()
{
    state_t *new;
    new = ((state_t *) INT_NEWAREA);
    new->pc = (unsigned int)test;
    new->sp = RAM_TOP;
    setSTATUS(STATUS_ENABLE_TIMER(getSTATUS()));
    setTIMER(50000*(*((unsigned int *)BUS_REG_TIME_SCALE)));
    tprint("First checkpoint\n");
    while(global);
    tprint("Hello world!\n");
    HALT();
}
