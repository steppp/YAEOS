#include "list.h"

//Se si usa solo questo commentare il parametro state_t in pcb.h


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




void main(){

    //TestInsertProcQ();
    //TestheadProcQ();
    //TestremoveProcQ();
    TestoutProcQ();
    //TestforallProcQ();

}
