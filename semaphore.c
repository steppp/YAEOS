#include <semaphore.h>
#include <list.h>

int insertBlocked(int *key,pcb_t *p)
{
    /*
       - calculate key hash
       - if the table entry is empty or does not contain the given key
          - if the freeSemds queue is not empty
              - allocate a new semaphore and insert in the table
          - else return -1
       - else insert p in the queue of key
       - return 0
    */
    int ind = hash(key);
    semd_t *entry;
    if ((entry = hashentry(semdhash[ind],key)) != NULL)
    {
        if (semdFree_h != NULL)
        {
            semd_t *new = semdFree_h;       /* Allocate a new semaphore and
                                               remove it from the freeSemd list*/
            semdFree_h = semdFree_h->s_next;    
            new->s_key = key;
            new->s_procQ = p;   // The queue is empty
            new->s_next = semdhash[ind];
            semdhash[ind] = new;
        }
        else
            return -1;
    }
    else
        enqueue(&entry->s_procQ,p); // Need to make sure that p->p_next == NULL
    return 0;
}

pcb_t *headBlocked(int *key)
{
    int ind = hash(key);
    semd_t *entry = hashentry(semdhash[ind],key);
    if (entry == NULL)
        return NULL;
    else
        return entry->s_procQ;
}

pcb_t *removeBlocked(int *key)
{
    int ind = hash(key);
    semd_t *entry = hashentry(semdhash[ind],key);
    if (entry == NULL)
        return NULL;
    else
    {
        pcb_t *ret = entry->s_procQ;
        entry->s_procQ = entry->s_procQ->p_next;
        if (entry->s_procQ == NULL)
        {
             /*remove the semaphore descriptor from the table and return it to the free smds
                                           */
            removeEntry(&semdhash[ind],entry);
            entry->s_next = semdFree_h;
            semdFree_h = entry;
        }

        return ret;
    }
}

void forallBlocked(int *key, void (*fun)(pcb_t *pcb, void *), void *arg)
{
    int ind = hash(key);
    semd_t *entry = hashentry(semdhash[ind],key);
    if (entry != NULL)
    {
        pcb_t *p = entry->s_procQ;
        callFun(p,fun,arg);
    }
}

void outChildBlocked(pcb_t *p)
{
    int ind = 0;
    semd_t *entry;
    entry = removePCB(p,&ind); /* ind will keep the index of the bucket list which contained the semaphore p was
    blocked on */
    if (entry != NULL && entry->s_procQ == NULL)
    {
        /*remove the semaphore descriptor from the table and return it to the free smds*/
        removeEntry(&semdhash[ind],entry);
        entry->s_next = semdFree_h;
        semdFree_h = entry;
    }
}

void initASL()
{
    semdFree_h = NULL;
    fillFreeSemd(0);
}

void fillFreeSemd(int i)
{
    if (i < MAXSEMD)
    {
        semd_table[i].s_next = semdFree_h;
        semdFree_h = &semd_table[i];
        fillFreeSemd(i+1);
    }
}

semd_t *removePCB(pcb_t *p,int *ind)
{
    if(*ind < ASHDSIZE)
    {
        semd_t *entry;
        entry = removePCBfromQueue(p,semdhash[*ind]);
        if (entry != NULL)
            return entry;
        else
        {
            *ind = *ind + 1;
            return removePCB(p,ind);
        }
    }
}

semd_t *removePCBfromQueue(pcb_t *p,semd_t *bucketlist)
{
    if(bucketlist != NULL)
    {
        if(outProcQ(&bucketlist->s_procQ,p) != NULL)
            return bucketlist;
        else
            return removePCBfromQueue(p,bucketlist->s_next);
    }
    else
        return NULL;
}

void callFun(pcb_t *p,void (*fun)(pcb_t *pcb, void *),void *arg)
{
    if(p != NULL)
    {
        fun(p,arg);
        callFun(p->p_next,fun,arg);
    }
}

semd_t *hashentry(semd_t *bucketlist,int *key)
{
    if (bucketlist == NULL)
        return NULL;
    else if (bucketlist->s_key == key)
        return bucketlist;
    else
        return hashentry(bucketlist->s_next,key);
}

int hash(int *key)
{
   return (int)ASHDSIZE*((long int)key*HASH_MULT_CONST-(long int)((long int)key*HASH_MULT_CONST));
}

void enqueue(pcb_t **queue,pcb_t *p)
{
    if(*queue == NULL)
        *queue = p;
    else
        enqueue(&((*queue)->p_next),p);
}

void removeEntry(semd_t **bucketlist,semd_t *entry)
{
    if (bucketlist == NULL || *bucketlist == NULL)
        return;
    else if(*bucketlist == entry)
        *bucketlist = (*bucketlist)->s_next;
    else
        removeEntry(&((*bucketlist)->s_next),entry);
}
