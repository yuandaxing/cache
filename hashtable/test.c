#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dict.h"
#include "randgen.h"
#include "log.h"
#define HASHSIZE 1000000000
int main()
{
	char *str1 = "HeapStringCopyKeyVal";
	dict *strdict = dictCreate(&dictTypeHeapStringCopyKeyValue,str1);
	char key[10], value[20];
	int i;
	int ret = 0;
	dictEntry *e;
	
	for(i = 0; i <HASHSIZE; i++){
		fillRandomString(9, key);
		fillRandomString(19, value);
		ret = dictAdd(strdict, key, value);
		if(ret == DICT_ERR){
			e = dictFind(strdict, key);
			log_write("key:%s, value:%s\n",(char *)e->key, 
					(char *)e->val); 
		}
		
	}
	//printf("%s\n", mallocRandomString(20));
	//printf("%s\n", mallocRandomString(20));
	//printf("%s\n", mallocRandomString(20));
	while(1) sleep(1);
	 
	return 0;
}
