//#include "../facilities/libuarm.h"
#include "/usr/include/uarm/libuarm.h"
#include "pcb.h"
#include "tree.h"

char *error="If this got printed something fishy has happened..\n\0";

int main(){

    tprint("Hello Wooooooorld!\n\0");

    HALT();

    tprint(error);

    return 0;
}
