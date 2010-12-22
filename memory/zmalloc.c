#include <stdlib.h>
#include <string.h>
#include <stdio.h>
/*all memory be used*/
#ifdef DEBUG
static size_t freed = 0;
static size_t mallocd = 0;
#endif
static size_t used_memory = 0;

void *zmalloc(size_t size) {
    void *ptr = malloc(size+sizeof(size_t));

    if (!ptr) return NULL;
    *((size_t*)ptr) = size;
    used_memory += size+sizeof(size_t);
#ifdef DEBUG 
	mallocd += size+sizeof(size_t);
#endif
    return (char*)ptr+sizeof(size_t);
}

void *zrealloc(void *ptr, size_t size) {
    void *realptr;
    size_t oldsize;
    void *newptr;

    if (ptr == NULL) return zmalloc(size);
    realptr = (char*)ptr-sizeof(size_t);
    oldsize = *((size_t*)realptr);
/*auto free ptr memory*/
    newptr = realloc(realptr,size+sizeof(size_t));
    if (!newptr) return NULL;
#ifdef DEBUG
	mallocd += size + sizeof(size_t);
	freed += oldsize + sizeof(size_t);
#endif
    *((size_t*)newptr) = size;
    used_memory -= oldsize;
    used_memory += size;
    return (char*)newptr+sizeof(size_t);
}

void zfree(void *ptr) {
    void *realptr;
    size_t oldsize;

    if (ptr == NULL) return;
    realptr = (char*)ptr-sizeof(size_t);
    oldsize = *((size_t*)realptr);
#ifdef DEBUG
	freed += oldsize + sizeof(size_t);
#endif
    used_memory -= oldsize+sizeof(size_t);
    free(realptr);
}

char *zstrdup(const char *s) {
    size_t l = strlen(s)+1;
    char *p = zmalloc(l);

    memcpy(p,s,l);
    return p;
}

size_t zmalloc_used_memory(void) {
    return used_memory;
}
#ifdef DEBUG
void  check_memory()
{
	fprintf(stdout, "malloced %u memory\n", mallocd);
	fprintf(stdout, "freed %u memory\n", freed);
	fprintf(stdout, "%u maybe lost or in memory\n", mallocd - freed);
}
#endif
