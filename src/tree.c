#include  <tree.h>


void insertChild(pcb_t *parent, pcb_t *p) {
	//if it has no child 
	if (parent->p_first_child == NULL) {
		//add as first child
		parent->p_first_child = p;
		p->p_sib = NULL;
	}
	//if it has a child instead add this as a sibling of his first child
	else _addAsLastSibl(parent->p_first_child, p);
	p->p_parent = parent;
}

void _addAsLastSibl(pcb_t *node, pcb_t *p) {
	//If node does not have siblings 
	if (node->p_sib == NULL){
		node->p_sib = p; //add p as the last sibling
	}

	else _addAsLastSibl(node->p_sib, p);
}

pcb_t *removeChild(pcb_t *p) {
	pcb_t *child = p->p_first_child;
	//If p has no child return null
	if (p->p_first_child == NULL) return NULL;
	//Else remove it from first child. Leave the tree node consistent.
	p->p_first_child = child->p_sib;
	child->p_sib = NULL;
	child->p_parent = NULL;

	return child;
}

pcb_t *outChild(pcb_t *p) {
	pcb_t *prev_p;
	//If p has no parent return null
	if (p->p_parent == NULL) 
		return NULL;
	//If p is the first child of his parent
	if (p->p_parent->p_first_child == p) {
		p->p_parent->p_first_child = p->p_sib;
	}
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
