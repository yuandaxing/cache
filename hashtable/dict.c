#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <limits.h>

#include "dict.h"
#include "zmalloc.h"

/* ---------------------------- Utility funcitons --------------------------- */

static void _dictPanic(const char *fmt, ...)
{
    va_list ap;

    va_start(ap, fmt);
    fprintf(stderr, "\nDICT LIBRARY PANIC: ");
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n\n");
    va_end(ap);
}

/* ------------------------- Heap Management Wrappers------------------------ */

static void *_dictAlloc(size_t size)
{
    void *p = zmalloc(size);
    if (p == NULL)
        _dictPanic("Out of memory");
    return p;
}

static void _dictFree(void *ptr) {
    zfree(ptr);
}

/* -------------------------- private prototypes ---------------------------- */

static int _dictExpandIfNeeded(dict *ht);
static unsigned long _dictNextPower(unsigned long size);
static int _dictKeyIndex(dict *ht, const void *key);
static int _dictInit(dict *ht, dictType *type, void *privDataPtr);

/* -------------------------- hash functions -------------------------------- */

/* Thomas Wang's 32 bit Mix Function */
unsigned int dictIntHashFunction(unsigned int key)
{
    key += ~(key << 15);
    key ^=  (key >> 10);
    key +=  (key << 3);
    key ^=  (key >> 6);
    key += ~(key << 11);
    key ^=  (key >> 16);
    return key;
}

/* Identity hash function for integer keys */
unsigned int dictIdentityHashFunction(unsigned int key)
{
    return key;
}

/* Generic hash function (a popular one from Bernstein).
 * I tested a few and this was the best. */
unsigned int dictGenHashFunction(const unsigned char *buf, int len) {
    unsigned int hash = 5381;

    while (len--)
        hash = ((hash << 5) + hash) + (*buf++); /* hash * 33 + c */
    return hash;
}

/* ----------------------------- API implementation ------------------------- */

/* Reset an hashtable already initialized with ht_init().
 * NOTE: This function should only called by ht_destroy(). */
static void _dictReset(dict *ht)
{
    ht->table = NULL;
    ht->size = 0;
    ht->sizemask = 0;
    ht->used = 0;
}

/* Create a new hash table */
dict *dictCreate(dictType *type,
        void *privDataPtr)
{
    dict *ht = _dictAlloc(sizeof(*ht));

    _dictInit(ht,type,privDataPtr);
    return ht;
}

/* Initialize the hash table */
int _dictInit(dict *ht, dictType *type,
        void *privDataPtr)
{
    _dictReset(ht);
    ht->type = type;
    ht->privdata = privDataPtr;
    return DICT_OK;
}

/* Resize the table to the minimal size that contains all the elements,
 * but with the invariant of a USER/BUCKETS ration near to <= 1 */
int dictResize(dict *ht)
{
    int minimal = ht->used;

    if (minimal < DICT_HT_INITIAL_SIZE)
        minimal = DICT_HT_INITIAL_SIZE;
    return dictExpand(ht, minimal);
}

/* Expand or create the hashtable */
int dictExpand(dict *ht, unsigned long size)
{
    dict n; /* the new hashtable */
    unsigned long realsize = _dictNextPower(size), i;

    /* the size is invalid if it is smaller than the number of
     * elements already inside the hashtable */
    if (ht->used > size)/*dictEntry of ht*/
        return DICT_ERR;

    _dictInit(&n, ht->type, ht->privdata);
    n.size = realsize;
    n.sizemask = realsize-1;
    n.table = _dictAlloc(realsize*sizeof(dictEntry*));

    /* Initialize all the pointers to NULL */
    memset(n.table, 0, realsize*sizeof(dictEntry*));

    /* Copy all the elements from the old to the new table:
     * note that if the old hash table is empty ht->size is zero,
     * so dictExpand just creates an hash table. */
    n.used = ht->used;
    for (i = 0; i < ht->size && ht->used > 0; i++) {
        dictEntry *he, *nextHe;

        if (ht->table[i] == NULL) continue;
        
        /* For each hash entry on this slot... */
        he = ht->table[i];
        while(he) {
            unsigned int h;

            nextHe = he->next;
            /* Get the new element index */
            h = dictHashKey(ht, he->key) & n.sizemask;
            he->next = n.table[h];
            n.table[h] = he;
            ht->used--;
            /* Pass to the next element */
            he = nextHe;
        }
    }
    assert(ht->used == 0);
    _dictFree(ht->table);

    /* Remap the new hashtable in the old */
    *ht = n;
    return DICT_OK;
}

/* Add an element to the target hash table */
int dictAdd(dict *ht, void *key, void *val)
{
    int index;
    dictEntry *entry;

    /* Get the index of the new element, or -1 if
     * the element already exists. */
    if ((index = _dictKeyIndex(ht, key)) == -1)
        return DICT_ERR;

    /* Allocates the memory and stores key */
    entry = _dictAlloc(sizeof(*entry));
    entry->next = ht->table[index];
    ht->table[index] = entry;

    /* Set the hash entry fields. */
    dictSetHashKey(ht, entry, key);/*dup the key and val*/
    dictSetHashVal(ht, entry, val);
    ht->used++;
    return DICT_OK;
}

/* Add an element, discarding the old if the key already exists */
int dictReplace(dict *ht, void *key, void *val)
{
    dictEntry *entry;

    /* Try to add the element. If the key
     * does not exists dictAdd will suceed. */
/*I think we can do some fix to make it faster*/
    if (dictAdd(ht, key, val) == DICT_OK)
        return DICT_OK;
    /* It already exists, get the entry */
    entry = dictFind(ht, key);
    /* Free the old value and set the new one */
    dictFreeEntryVal(ht, entry);
    dictSetHashVal(ht, entry, val);
    return DICT_OK;
}

/* Search and remove an element */
static int dictGenericDelete(dict *ht, const void *key, int nofree)
{
    unsigned int h;
    dictEntry *he, *prevHe;

    if (ht->size == 0)
        return DICT_ERR;
    h = dictHashKey(ht, key) & ht->sizemask;
    he = ht->table[h];

    prevHe = NULL;
    while(he) {
        if (dictCompareHashKeys(ht, key, he->key)) {
            /* Unlink the element from the list */
            if (prevHe)
                prevHe->next = he->next;
            else
                ht->table[h] = he->next;
            if (!nofree) {
                dictFreeEntryKey(ht, he);
                dictFreeEntryVal(ht, he);
            }
            _dictFree(he);
            ht->used--;
            return DICT_OK;
        }
        prevHe = he;
        he = he->next;
    }
    return DICT_ERR; /* not found */
}

int dictDelete(dict *ht, const void *key) {
    return dictGenericDelete(ht,key,0);
}

int dictDeleteNoFree(dict *ht, const void *key) {
    return dictGenericDelete(ht,key,1);
}

/* Destroy an entire hash table */
int _dictClear(dict *ht)
{
    unsigned long i;

    /* Free all the elements */
    for (i = 0; i < ht->size && ht->used > 0; i++) {
        dictEntry *he, *nextHe;

        if ((he = ht->table[i]) == NULL) continue;
        while(he) {
            nextHe = he->next;
            dictFreeEntryKey(ht, he);
            dictFreeEntryVal(ht, he);
            _dictFree(he);
            ht->used--;
            he = nextHe;
        }
    }
    /* Free the table and the allocated cache structure */
    _dictFree(ht->table);
    /* Re-initialize the table */
    _dictReset(ht);
    return DICT_OK; /* never fails */
}

/* Clear & Release the hash table */
void dictRelease(dict *ht)
{
    _dictClear(ht);
    _dictFree(ht);
}

dictEntry *dictFind(dict *ht, const void *key)
{
    dictEntry *he;
    unsigned int h;

    if (ht->size == 0) return NULL;
    h = dictHashKey(ht, key) & ht->sizemask;
    he = ht->table[h];
    while(he) {
        if (dictCompareHashKeys(ht, key, he->key))
            return he;
        he = he->next;
    }
    return NULL;
}

dictIterator *dictGetIterator(dict *ht)
{
    dictIterator *iter = _dictAlloc(sizeof(*iter));

    iter->ht = ht;
    iter->index = -1;
    iter->entry = NULL;
    iter->nextEntry = NULL;
    return iter;
}

dictEntry *dictNext(dictIterator *iter)
{
    while (1) {
        if (iter->entry == NULL) {
            iter->index++;
            if (iter->index >=
                    (signed)iter->ht->size) break;
            iter->entry = iter->ht->table[iter->index];
        } else {
            iter->entry = iter->nextEntry;
        }
        if (iter->entry) {
            /* We need to save the 'next' here, the iterator user
             * may delete the entry we are returning. */
            iter->nextEntry = iter->entry->next;
            return iter->entry;
        }
    }
    return NULL;
}

void dictReleaseIterator(dictIterator *iter)
{
    _dictFree(iter);
}

/* Return a random entry from the hash table. Useful to
 * implement randomized algorithms */
dictEntry *dictGetRandomKey(dict *ht)
{
    dictEntry *he;
    unsigned int h;
    int listlen, listele;

    if (ht->used == 0) return NULL;
    do {
        h = random() & ht->sizemask;
        he = ht->table[h];
    } while(he == NULL);

    /* Now we found a non empty bucket, but it is a linked
     * list and we need to get a random element from the list.
     * The only sane明智的 way to do so is to count the element and
     * select a random index. */
    listlen = 0;
    while(he) {
        he = he->next;
        listlen++;
    }
    listele = random() % listlen;
    he = ht->table[h];
    while(listele--) he = he->next;
    return he;
}

/* ------------------------- private functions ------------------------------ */

/* Expand the hash table if needed */
static int _dictExpandIfNeeded(dict *ht)
{
    /* If the hash table is empty expand it to the intial size,
     * if the table is "full" dobule its size. */
    if (ht->size == 0)
        return dictExpand(ht, DICT_HT_INITIAL_SIZE);
    if (ht->used == ht->size)/*really this is puzzling*/
        return dictExpand(ht, ht->size*2);
    return DICT_OK;
}
/*is it to big*/
/* Our hash table capability is a power of two */
static unsigned long _dictNextPower(unsigned long size)
{
    unsigned long i = DICT_HT_INITIAL_SIZE;

    if (size >= LONG_MAX) return LONG_MAX;
    while(1) {
        if (i >= size)
            return i;
        i *= 2;
    }
}

/* Returns the index of a free slot that can be populated with
 * an hash entry for the given 'key'.
 * If the key already exists, -1 is returned. */
static int _dictKeyIndex(dict *ht, const void *key)
{/*the key should be unique*/
    unsigned int h;
    dictEntry *he;

    /* Expand the hashtable if needed */
    if (_dictExpandIfNeeded(ht) == DICT_ERR)
        return -1;
    /* Compute the key hash value */
    h = dictHashKey(ht, key) & ht->sizemask;
    /* Search if this slot does not already contain the given key */
    he = ht->table[h];
    while(he) {
        if (dictCompareHashKeys(ht, key, he->key))
            return -1;
        he = he->next;
    }
    return h;
}

void dictEmpty(dict *ht) {
    _dictClear(ht);
}

#define DICT_STATS_VECTLEN 50
void dictPrintStats(dict *ht) {
    unsigned long i, slots = 0, chainlen, maxchainlen = 0;
    unsigned long totchainlen = 0;
    unsigned long clvector[DICT_STATS_VECTLEN];

    if (ht->used == 0) {
        printf("No stats available for empty dictionaries\n");
        return;
    }

    for (i = 0; i < DICT_STATS_VECTLEN; i++) clvector[i] = 0;
    for (i = 0; i < ht->size; i++) {
        dictEntry *he;

        if (ht->table[i] == NULL) {
            clvector[0]++;
            continue;
        }
        slots++;
        /* For each hash entry on this slot... */
        chainlen = 0;
        he = ht->table[i];
        while(he) {
            chainlen++;
            he = he->next;
        }
        clvector[(chainlen < DICT_STATS_VECTLEN) ? chainlen : (DICT_STATS_VECTLEN-1)]++;
        if (chainlen > maxchainlen) maxchainlen = chainlen;
        totchainlen += chainlen;
    }
    printf("Hash table stats:\n");
    printf(" table size: %ld\n", ht->size);
    printf(" number of elements: %ld\n", ht->used);
    printf(" different slots: %ld\n", slots);
    printf(" max chain length: %ld\n", maxchainlen);
    printf(" avg chain length (counted): %.02f\n", (float)totchainlen/slots);
    printf(" avg chain length (computed): %.02f\n", (float)ht->used/slots);
    printf(" Chain length distribution:\n");
    for (i = 0; i < DICT_STATS_VECTLEN-1; i++) {
        if (clvector[i] == 0) continue;
        printf("   %s%ld: %ld (%.02f%%)\n",(i == DICT_STATS_VECTLEN-1)?">= ":"", i, clvector[i], ((float)clvector[i]/ht->size)*100);
    }
}

/* ----------------------- StringCopy Hash Table Type ------------------------*/

static unsigned int _dictStringCopyHTHashFunction(const void *key)
{
    return dictGenHashFunction(key, strlen(key));
}

static void *_dictStringCopyHTKeyDup(void *privdata, const void *key)
{
    int len = strlen(key);
    char *copy = _dictAlloc(len+1);
    DICT_NOTUSED(privdata);

    memcpy(copy, key, len);
    copy[len] = '\0';
    return copy;
}

static void *_dictStringKeyValCopyHTValDup(void *privdata, const void *val)
{
    int len = strlen(val);
    char *copy = _dictAlloc(len+1);
    DICT_NOTUSED(privdata);

    memcpy(copy, val, len);
    copy[len] = '\0';
    return copy;
}

static int _dictStringCopyHTKeyCompare(void *privdata, const void *key1,
        const void *key2)
{
    DICT_NOTUSED(privdata);

    return strcmp(key1, key2) == 0;
}

static void _dictStringCopyHTKeyDestructor(void *privdata, void *key)
{
    DICT_NOTUSED(privdata);

    _dictFree((void*)key); /* ATTENTION: const cast */
}

static void _dictStringKeyValCopyHTValDestructor(void *privdata, void *val)
{
    DICT_NOTUSED(privdata);

    _dictFree((void*)val); /* ATTENTION: const cast */
}
//no val dup, just copy key
dictType dictTypeHeapStringCopyKey = {
    _dictStringCopyHTHashFunction,        /* hash function */
    _dictStringCopyHTKeyDup,              /* key dup */
    NULL,                               /* val dup */
    _dictStringCopyHTKeyCompare,          /* key compare */
    _dictStringCopyHTKeyDestructor,       /* key destructor */
    NULL                                /* val destructor */
};

/* This is like StringCopy but does not auto-duplicate the key.
 * It's used for intepreter's shared strings. */
dictType dictTypeHeapStrings = {
    _dictStringCopyHTHashFunction,        /* hash function */
    NULL,                               /* key dup */
    NULL,                               /* val dup */
    _dictStringCopyHTKeyCompare,          /* key compare */
    _dictStringCopyHTKeyDestructor,       /* key destructor, should be NULL */
    NULL                                /* val destructor */
};

/* This is like StringCopy but also automatically handle dynamic
 * allocated C strings as values. */
dictType dictTypeHeapStringCopyKeyValue = {
    _dictStringCopyHTHashFunction,        /* hash function */
    _dictStringCopyHTKeyDup,              /* key dup */
    _dictStringKeyValCopyHTValDup,        /* val dup */
    _dictStringCopyHTKeyCompare,          /* key compare */
    _dictStringCopyHTKeyDestructor,       /* key destructor */
    _dictStringKeyValCopyHTValDestructor, /* val destructor */
};
