#include "pcbFree.h"


pcb_t *insertLastElements(int currentIndex) {
    if (currentIndex < MAXPROC) {
	/*
	 * extraxt the element at the specified index from the table
	 * and set its next element as the element of index `currentIndex + 1`
	 * inside the array pcbFree_table  
	 */
        pcb_t *p = &pcbFree_table[currentIndex];
        p->p_next = insertLastElements(currentIndex + 1);

        return p;
    }

    /*
     * return null if there are no more elements in the freePcb_table array
     */
    return 0;
}

void append(pcb_t *p, pcb_t *currentElem, int elementsCount) {
    if (elementsCount < MAXPROC) {  // c'è ancora spazio nell'array pcbFree
        if (!currentElem->p_next) {
	    /*
	     * if last element, set the pointer to the next element of the new 
	     * element to be inserted to null and set the new element as the next
	     * of the last element in the pcbFree list
	     */
            p->p_next = 0;
            currentElem->p_next = p;
            return;
        }

	/*
	 * otherwise (not yet encountered the last element of the pcbFree list)
	 * perform a recursive call to this method to advance by one element
	 * in the list
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
        return;

    if (!pcbFree_h) {
	/*
	 * if head is null then the list is empty
	 * assign the desired value to the head and set the next element's
	 * value in the list to null
        pcbFree_h = p;
        pcbFree_h->p_next = 0;

        return;
    }

    append(p, pcbFree_h, 1);
}

pcb_t *allocPcb() {
    pcb_t *p = pcbFree_h;

    if (p == NULL) return NULL;

    pcbFree_h = pcbFree_h->p_next;

    p->p_next = NULL;
    p->p_parent = NULL;
    p->p_first_child = NULL;
    p->p_sib = NULL;

    //state_t np_s = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

    //p->p_s = state_t{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
    p->p_s = (state_t) {0};
    p->p_priority = 0;
    p->p_semKey = NULL;
    //*p = (pcb_t) {0};
    return p;
}
