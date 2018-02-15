#ifndef TREE_H
#define TREE_H
#include "tree.h"
#endif

void insertChild(pcb_t *parent, pcb_t *p) {
	//if it has no child 
	if (parent->p_first_child == NULL) {
		parent->p_first_child = p;//add as first child
	}
	//if it has a child instead...
	else _addAsLastSibl(parent->p_first_child, p);

	p->p_parent = parent;
}

/**
* Recursive function. Adds p as the last sibling of node.
*/
void _addAsLastSibl(pcb_t *node, pcb_t *p) {
	//If node does not have siblings anymore
	if (node->p_sib == NULL)
		node->p_sib = p; //I add p as the last sibling

	else _addAsLastSibl(node->p_sib, p);
}

pcb_t *removeChild(pcb_t *p) {
	pcb_t *child = p->p_first_child;
	//if p has no child return null
	if (child == NULL) return NULL;

	p->p_first_child = child->p_sib;
	child->p_parent = NULL;
	return child;
}

pcb_t *outChild(pcb_t *p) {
	pcb_t *prev_p;
	//If p has no parent return null
	if (p->p_parent == NULL) 
		return NULL;
	//If p is the first child of his parent
	if (p->p_parent->p_first_child = p) {
		p->p_parent->p_first_child = p->p_sib;
	}
	//Else if is not the first child of his parent...
	else {
		//Search for his previous silbling 
		prev_p = _fetchPreviousSibling(p, p->p_parent->p_first_child); 
		//Previous sibling's sibling is the sibling of p (i am excluding p from the list)
		prev_p->p_sib = p->p_sib;
	}
	//p has no parent anymore
	p->p_parent = NULL;
	return p;
}

pcb_t *_fetchPreviousSibling(pcb_t *p, pcb_t *next) {
	if (next->p_sib == NULL)
		return NULL;
	if (next->p_sib == p)
		return next;
	return _fetchPreviousSibling(p, next->p_sib);
}