#include "pcbFree.h"

pcb_t *insertLastElements(int currentIndex) {
    if (currentIndex < MAXPROC) {   // non ho ancora superato la capacità
        /* estraggo l'elemento corrente dalla lista e gli assegno come
         * successivo l'elemento con indice appena superiore all'interno
         * dell'array pcbFree_table
         */
        pcb_t *p = &pcbFree_table[currentIndex];
        p->p_next = insertLastElements(currentIndex + 1);

        return p;
    }

    /* se ho finito gli elementi nella pcbFree_table allora torno un
     * puntatore vuoto
     */
    return 0;
}

void append(pcb_t *p, pcb_t *currentElem, int elementsCount) {
    if (elementsCount < MAXPROC) {  // c'è ancora spazio nell'array pcbFree
        if (!currentElem->p_next) {
            /*
             * Se sono arrivato all'ultimo elemento (aka il puntatore
             * all'elemento successivo è nullo) allora imposto il puntatore
             * all'elemento successivo del nuovo elemento da inserire a nullo
             * ed imposto il nuovo elemento come il successivo dell'ultimo
             * elemento nella lista pcbFree
             */
            p->p_next = 0;
            currentElem->p_next = p;
            return;
        }

        /*
         * Altrimenti (non sono ancora arrivato all'ultimo elemento
         * della lista pcbFree) faccio una chiamata ricorsiva a questo metodo
         * per avanzare di un elemento nella tabella dei pcbFree
         */
        append(p, currentElem->p_next, elementsCount + 1);
    }

    return;
}

void initPcbs() {
    pcbFree_h = insertLastElements(0);
}

void freePcb(pcb_t *p) {
    if (!p)
        // se p è nullo non faccio alcunché
        return;

    if (!pcbFree_h) {
        /*
         * Se la testa è nulla allora significa che la lista è vuota
         * quindi assegno il valore desiderato alla testa e assegno
         * il valore nullo al prossimo elemento della lista.
         */
        pcbFree_h = p;
        pcbFree_h->p_next = 0;

        return;
    }

    append(p, pcbFree_h, 1);
}

pcb_t *allocPcb() {
    pcb_t *p = pcbFree_h;

    if (p == NULL)
        return NULL;

    pcbFree_h = pcbFree_h->p_next;

    p->p_next = NULL;
    p->p_parent = NULL;
    p->p_first_child = NULL;
    p->p_sib = NULL;
    //p->p_s = (state_t) {0};
    p->p_priority = 0;
    p->p_semKey = NULL;

    return p;
}