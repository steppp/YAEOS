//for trees
#ifndef PCB_H
#define PCB_H
#include "pcb.h"
#endif

//defining null const
#ifndef NULL
#define NULL 0
#endif

/**
* Adds p as a child of parent
*/
void 	insertChild(pcb_t *parent, pcb_t *p);

/**
* Removes the first child of p. 
* If p has no childs, this function returns NULL, else returns the removed child.
*/
pcb_t 	*removeChild(pcb_t *p);

/**
* Removes p from the list of childrens of his parents
* Returns NULL if p does not exists in the list of his parent (IMPOSSIBLE!! CORRUPTED TREE???)
* Returns p if the removal has succeded
*/
pcb_t 	*outChild(pcb_t *p);

//adds the new child p as last sibling in node
void 	_addAsLastSibl(pcb_t *node, pcb_t *p);

/**
* Recursive function. Search the previous sibling of p.
* Returns the previous sibling of p in list if exists, otherwise returns NULL
* Tracks the next pcb: at any funcion recursion, next will be modified with the actual pcb
*/
pcb_t 	*_fetchPreviousSibling(pcb_t *p, pcb_t *next);



