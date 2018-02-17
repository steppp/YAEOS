#ifndef LIBUARM_H
#define LIBUARM_H
#include "/usr/include/uarm/libuarm.h"
#endif

#include "list.h"
#ifndef PCB_H
#define PCB_H
#include "pcb.h"
#endif

#ifndef TREE_H
#define TREE_H 
#include "tree.h"
#endif

//defining null const
#ifndef NULL
#define NULL 0
#endif
//Se si usa solo questo commentare il parametro state_t in pcb.h

char *error="If this got printed something fishy has happened..\n\0";

void TestInsertProcQ(){
    //Test InsertProQ
    printf("Test InsertProQ\n");

    pcb_t ** testa=malloc(sizeof(pcb_t));
    pcb_t * elem=malloc(sizeof(pcb_t));
    pcb_t * elem2=malloc(sizeof(pcb_t));
    pcb_t * elem3=malloc(sizeof(pcb_t));

    elem->priority=10;
    printf("Indirizzo elem priorità media\n");
    printf("%p\n",elem);

    printf("Test inserimento se lista e' vuota\n");
    printf("Prima inserimento\n");
    printf("%p\n",*testa);
    insertProcQ(testa,elem);
    printf("Dopo inserimento\n");
    printf("%p\n",*testa);
    

    printf("Test inserimento priorità alta e poi bassa\n");
    elem2->priority=5;
    printf("Indirizzo elem2 priorità alta\n");
    printf("%p\n",elem2);

    elem3->priority=1;
    
    printf("Indirizzo elem3 priorità bassa\n");
    printf("%p\n",elem3);

    
    insertProcQ(testa,elem2);
    insertProcQ(testa,elem3);
    

    printf("Dopo inserimento(Dovrebbe essere = elem2)\n");
    printf("%p\n",*testa);
    printf("%d\n",(*testa)->priority);

    printf("Scannerizzo tutta la lista stampando le priorità\n");
    pcb_t *scan=(*testa);
    while(scan!=NULL){
        printf("Priorita %d\n",scan->priority);
        scan=scan->p_next;
    }
}


void TestheadProcQ(){
    printf("Test headProQ\n");
    pcb_t ** testa=malloc(sizeof(pcb_t));
    pcb_t * elem=malloc(sizeof(pcb_t));
    pcb_t * elem2=malloc(sizeof(pcb_t));
    
    
    pcb_t * printer=malloc(sizeof(pcb_t));
    printer=headProcQ(*testa);
    printf("Dovrebbe essere nil: %p\n",printer);

    elem->priority=1;
    insertProcQ(testa,elem);
    elem2->priority=10;
    insertProcQ(testa,elem2);
    printf("Indirizzo elem2\n");
    printf("%p\n",elem2);
    
    printer=headProcQ(*testa);
    printf("Dovrebbe essere = indirizzo elem 2: %p\n",printer);
    

}

void TestremoveProcQ(){
    printf("Test removeProQ\n");

    pcb_t ** testa=malloc(sizeof(pcb_t));
    pcb_t * elem=malloc(sizeof(pcb_t));
    pcb_t * elem2=malloc(sizeof(pcb_t));
    
    
    
    pcb_t * printer=malloc(sizeof(pcb_t));
    printer=removeProcQ(testa);
    printf("Dovrebbe essere nil: %p\n",printer);

    elem->priority=10;
    insertProcQ(testa,elem);
    
    printf("Indirizzo elem\n");
    printf("%p\n",elem);
    elem2->priority=1;
    insertProcQ(testa,elem2);
    printf("Indirizzo elem2\n");
    printf("%p\n",elem2);
    
    printf("Rimuove la testa due volte");
    printer=removeProcQ(testa);
    printf("Dovrebbe essere = indirizzo elem: %p\n",printer);
    printf("Dovrebbe essere = indirizzo elem2: %p\n",*testa);
    printer=removeProcQ(testa);
    printf("Dovrebbe essere = indirizzo elem2: %p\n",printer);
    printf("Dovrebbe essere = nil: %p\n",*testa);
}


void TestoutProcQ(){
    printf("Test outProQ\n");
    pcb_t ** testa=malloc(sizeof(pcb_t));
    pcb_t * elem=malloc(sizeof(pcb_t));
    pcb_t * elem2=malloc(sizeof(pcb_t));
    pcb_t * elem3=malloc(sizeof(pcb_t));
    
    
    pcb_t * printer=malloc(sizeof(pcb_t));
    printer=outProcQ(testa,elem);
    printf("Dovrebbe essere nil: %p\n",printer);

    elem->priority=10;
    insertProcQ(testa,elem);
    printf("Indirizzo elem\n");
    printf("%p\n",elem);
    elem2->priority=5;
    insertProcQ(testa,elem2);
    printf("Indirizzo elem2\n");
    printf("%p\n",elem2);
    elem3->priority=1;
    insertProcQ(testa,elem3);
    printf("Indirizzo elem3\n");
    printf("%p\n",elem3);
    
    printf("Scansione iniziale\n");
    pcb_t *scan=(*testa);
    while(scan!=NULL){
        printf("Priorita %d\n",scan->priority);
        scan=scan->p_next;
    }
    
    printf("Rimozione di elem due volte per ottenere nil alla seconda\n");
    printer=outProcQ(testa,elem);
    printf("Dovrebbe essere = indirizzo elem: %p\n",printer);
    printer=outProcQ(testa,elem);
    printf("Dovrebbe essere nil: %p\n",printer);
    
    printf("Scansione per vedere se e' stato tolto\n");
    scan=(*testa);
    while(scan!=NULL){
        printf("Priorita %d\n",scan->priority);
        scan=scan->p_next;
    }
}


void incrementauno(pcb_t * pcb, void * a){
//La funzione usate per il test di forallProcQ
    int *counter=a;
    pcb->priority+=(*counter);
}

void TestforallProcQ(){
    printf("Test forallProcQ\n");
	pcb_t ** testa=malloc(sizeof(pcb_t));
    pcb_t * elem=malloc(sizeof(pcb_t));
    pcb_t * elem2=malloc(sizeof(pcb_t));
    pcb_t * elem3=malloc(sizeof(pcb_t));
    
    elem->priority=10;
    insertProcQ(testa,elem);
    printf("Indirizzo elem\n");
    printf("%p\n",elem);

    elem2->priority=5;
    insertProcQ(testa,elem2);
    printf("Indirizzo elem2\n");
    printf("%p\n",elem2);

    elem3->priority=1;
    insertProcQ(testa,elem3);
    printf("Indirizzo elem3\n");
    printf("%p\n",elem3);
    
    printf("Scansione con le priorità normali\n");
    pcb_t *scan=(*testa);
    while(scan!=NULL){
        printf("Priorita %d\n",scan->priority);
        scan=scan->p_next;
    }
    int *a=malloc(sizeof(int));
    *a=5;
    forallProcQ((*testa),incrementauno,a);
    
    printf("Scansione dopo aver chiamato la funzione\n");
    scan=(*testa);
	 while(scan!=NULL){
        printf("Priorita %d\n",scan->priority);
        scan=scan->p_next;
    }
}

void printTree(pcb_t *node, int level) {
    int i;
    char c = '0';
    for (i = 0; i < level; i++) 
        tprint("\t\0");
    c = c + node->priority;
    tprint(&c);
    tprint(" prnt=\0");
    c = '0';
    c = c + node->p_parent->priority;
    tprint(&c);
    tprint("\n\0");
    if (node->p_first_child != NULL)
        printTree(node->p_first_child, level +1);
    if (node->p_sib != NULL)
        printTree(node->p_sib, level);

}

int chelloMain(){

    tprint("Hello Woeorld!\n\0");

    pcb_t   p1   = {NULL, NULL, NULL, NULL, NULL, 1, NULL};
    pcb_t   p2   = {NULL, NULL, NULL, NULL, NULL, 2, NULL};
    pcb_t   p3   = {NULL, NULL, NULL, NULL, NULL, 3, NULL};
    pcb_t   p4   = {NULL, NULL, NULL, NULL, NULL, 4, NULL};
    pcb_t   p5   = {NULL, NULL, NULL, NULL, NULL, 5, NULL};
    pcb_t   p6   = {NULL, NULL, NULL, NULL, NULL, 6, NULL};

    pcb_t   princ= {NULL, NULL, &p1, NULL, NULL, 0, NULL};

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



void main(){

    //TestInsertProcQ();
    //TestheadProcQ();
    //TestremoveProcQ();
    //TestoutProcQ();
    //TestforallProcQ();
    chelloMain();
}
