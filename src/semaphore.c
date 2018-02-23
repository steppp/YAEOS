#include  <semaphore.h>
#include  <list.h>

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
    if ((entry = hashentry(semdhash[ind],key)) == NULL)
    {
        if (semdFree_h != NULL)
        {
            semd_t *new = semdFree_h;       /* Allocate a new semaphore and
                                               remove it from the freeSemd list*/
            semdFree_h = semdFree_h->s_next;    
            new->s_key = key;
            new->s_procQ = NULL;
            new->s_next = semdhash[ind]; // Adding the semaphore descriptor to the bucket list 
            semdhash[ind] = new;

            entry = new;
        }
        else
            return -1;
    }

    p->p_semKey = key;
    insertProcQ(&entry->s_procQ,p);
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
        hashCleanup(entry,ind);
        ret->p_semKey = NULL;
        return ret;
    }
}

void forallBlocked(int *key, void fun(pcb_t *pcb, void *), void *arg)
{
    int ind = hash(key);
    semd_t *entry = hashentry(semdhash[ind],key);
    if (entry != NULL)
        forallProcQ(entry->s_procQ,fun,arg);
}

void outChildBlocked(pcb_t *p)
{
    if(p->p_semKey != NULL)
    {
        int ind = hash(p->p_semKey);
        semd_t *entry = hashentry(semdhash[ind],p->p_semKey);
        if(entry != NULL)
        {
            outProcQ(&entry->s_procQ,p);
            p->p_semKey = NULL;
            hashCleanup(entry,ind);
        }
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
        if (i < ASHDSIZE)       /* while semdFree is filled the hash table is initialized */
            semdhash[i] = NULL;
        semd_table[i].s_next = semdFree_h;
        semdFree_h = &semd_table[i];
        fillFreeSemd(i+1);
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
    double n = ((int)key)*HASH_MULT_CONST;
    return (int)(ASHDSIZE*(n - ((int)n))) % ASHDSIZE;
   /*
DISCLAIMER: the modulo operation shouldn't be necessary: x - floor(x) is a number
greater than or equal to 0 and less than 1, so the number floor(m*(iC - floor(iC)))
should be between 0 and m - 1, with m being ASHDSIZE, C being HASH_MULT_CONST and
i being the key.  However it seems like this calculation sometimes yields numbers
greater or equal to ASHDSIZE, which is obviously wrong. Now, the modulo operation
at the end patches this problem, but it's not an optimal solution.  Still it
should provide a uniform distribution of the keys.
      */
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

void hashCleanup(semd_t *entry,int ind)
{
    if (entry != NULL && entry->s_procQ == NULL)
    {
        removeEntry(&semdhash[ind],entry);
        entry->s_next = semdFree_h;
        semdFree_h = entry;
    }
}
