/*
 *	author:ydx
 *	email:1179923349@qq.com
 *	refernece:redis
 */
#ifndef ZMALLOC_H
#define ZMALLOC_H

void *zmalloc(size_t size);
void *zrealloc(void *ptr, size_t size);
void zfree(void *ptr);
char *zstrdup(const char *s);
size_t zmalloc_used_memory(void);

#ifdef DEBUG
void check_memory();
#endif

#endif 
