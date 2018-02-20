//#include "../facilities/libuarm.h"
#include "/usr/include/uarm/libuarm.h"

#include <uARMconst.h>
#include <uARMtypes.h>

#include "pcb.h"
#include "tree.h"

//defining null const
#ifndef NULL
#define NULL 0
#endif

char *error="If this got printed something fishy has happened..\n\0";

void printTree(pcb_t *node, int level) {
	int i;
	char c = '0';
	for (i = 0; i < level; i++) 
		tprint("\t\0");
	c = c + ((int)node)%93 +33;
	tprint(&c);
	tprint(" prnt=\0");
	c = '0';
	c = c + ((int)node->p_parent)%93 +33;
	tprint(&c);
	tprint("\n\0");
	if (node->p_first_child != NULL)
		printTree(node->p_first_child, level +1);
	if (node->p_sib != NULL)
		printTree(node->p_sib, level);

}


/*int mymain(){

    tprint("Hello Woeorld!\n\0");

    pcb_t	p1	 = {NULL, NULL, NULL, NULL, NULL, 1, NULL};
    pcb_t 	p2	 = {NULL, NULL, NULL, NULL, NULL, 2, NULL};
    pcb_t 	p3	 = {NULL, NULL, NULL, NULL, NULL, 3, NULL};
    pcb_t 	p4	 = {NULL, NULL, NULL, NULL, NULL, 4, NULL};
    pcb_t 	p5	 = {NULL, NULL, NULL, NULL, NULL, 5, NULL};
    pcb_t 	p6	 = {NULL, NULL, NULL, NULL, NULL, 6, NULL};

    pcb_t 	princ= {NULL, NULL, &p1, NULL, NULL, 0, NULL};

    p1.p_sib = &p2;
    p2.p_sib = &p3;

    p1.p_parent = &princ;
    p2.p_parent = &princ;
    p3.p_parent = &princ;

    insertChild(&p1, &p4);
    insertChild(&p1, &p5);
    insertChild(&p5, &p6);


    tprint("Now starting printing the tree yeee\n\0");
    printTree(&princ, 0);

    outChild(&p1);

    tprint("Now starting printing the tree yeee\n\0");
    printTree(&princ, 0);
    printTree(&p1, 0);

    tprint("Hello Woeorld! At the end\n\0");

    HALT();

    tprint(error);

    return 0;
}
*/
