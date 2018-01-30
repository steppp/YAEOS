Questo branch contiene i file per la gestione della lista dei PCB free, ovvero di tutti quei PCB
che sono liberi o inutilizzati. Le funzioni implementate sono:

- pcbFree -> lista dei processi che sono liberi o inutilizzati
- pcb_t *pcbFree_h -> testa della lista pcbFree
- pcb_t pcbFree_table[MAXPROC] -> array di PCB di dimensione MAXPROC
- void initPcbs() -> inizializza la pcbFree in modo da contenere tutti gli elementi della pcbFree_table
        questo metodo deve essere chiamato una volta sola in fase di inizializzazione della struttura dati
- void freePcb(pcb_t *p) -> inserisce il PCB puntato da p nella lista dei PCB liberi (pcbFree)
