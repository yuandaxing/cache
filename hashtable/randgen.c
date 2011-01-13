#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "dict.h"
//#define STRLEN 10
static initseed = 0;
const char strings[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static char *getRandomString(int len, char *str)
{
        char *s = str;
        size_t slen = strlen((const char *)strings);
        int i = 0;
	if(initseed == 0){
		initseed = 1;
		srand((unsigned)time(NULL));
	}
        if(s == NULL)
                s = (char *)malloc(len + 1);
        if(s == NULL){
                printf("cannot generator random character\n");
                exit(0);
        }

        for(i = 0; i < len; i++){
                s[i] = strings[rand()%slen];
        }
	s[i] = '\0';
        return s;

}
char *mallocRandomString(int len)
{
        return getRandomString(len, NULL);
}

void fillRandomString(int len, char *str)
{
        if(str == NULL){
                printf("argument 2 cannot be NULL\n");
                exit(0);
        }
        getRandomString(len, str);
}

