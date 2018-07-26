#include  <pcb.h>
#include <libuarm.h>

void insertFirst(pcb_t *p) {
    if (!p)
        return;
    p->p_next = pcbFree_h;
    pcbFree_h = p;
}

void insertLastElements(int currentIndex) {
    if (currentIndex < MAXPROC) {
	/*
	 * Extract the element at the specified index from the table
	 * and insert it in the first position of the pcbFree_h list
	 * then call recursively the function until the index go past
     * the dimension of pcbFree_table
	 */
        pcb_t *p = &pcbFree_table[currentIndex];
        insertFirst(p);
        insertLastElements(currentIndex + 1);
    }
}

void initPcbs() {
    insertLastElements(0);
}

void freePcb(pcb_t *p) {
    /* Insert the new pcb in first position */
    insertFirst(p);
}

pcb_t *allocPcb() {
    pcb_t *p = pcbFree_h;/* p is the head of pcb list */
    /* If the head is NULL return NULL */
    if (p == NULL) return NULL;
    /* The head is the next element of the head */
    pcbFree_h = pcbFree_h->p_next;
    /* Setting all pcb_t fields to NULL/0 */
    p->p_next = NULL;
    p->p_parent = NULL;
    p->p_first_child = NULL;
    p->p_sib = NULL;
    p->p_s = (state_t) {0};
    p->p_priority = p->old_priority = 0;
    p->p_semKey = NULL;

    return p;
}
