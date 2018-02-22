/*
 * Questo header descrive la struttura della lista
 * pcbFree, che contiene tutti i PCB che sono liberi
 * o inutilizzati.
 *
 * */

#ifndef PCBFREE_H
#define PCBFREE_H

#include <uARMconst.h>
#include <uARMtypes.h>

#include  <pcb.h>
#include  <const.h>

/*
 *
 */
pcb_t *pcbFree_h;

/*
 *
 */
pcb_t pcbFree_table[MAXPROC];


/*
 * Inizializza la lista in modo da contenere tutti gli elementi
 * della pcbFree_table. Deve essere chiamato una sola volta in
 * fase di inizializzazione dei dati.
 */
void initPcbs();

/*
 * Inserisce il PCB puntato da p nella lista dei PCB liberi
 * (pcbFree)
 */
void freePcb(pcb_t *p);

/*
 *
 */
pcb_t *allocPcb();

#endif //PCBFREE_H
