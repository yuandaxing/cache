#include <stdio.h>
#include "zmalloc.h"

int main()
{
	int *p = zmalloc(sizeof(int) * 20);
	int i;

	for(i = 0; i < 20; i++)
		p[i] = i;
	p = zrealloc(p, sizeof(int) * 40);
	for(i = 0; i < 20; i++)
		printf("%d\n", p[i]);
	
	printf("used memory %d\n", zmalloc_used_memory());

	char *s =  zstrdup("what a fabulous malloc\n");		

	check_memory();
	printf("used memory %d\n", zmalloc_used_memory());

	zfree(p);
	zfree(s);
	
	printf("used memory %d\n", zmalloc_used_memory());
	check_memory();
	return 0;
}


