#include <stdio.h>
#include "log.h"
#include <stdlib.h>
int main()
{
	while(1){
		sleep(1);
		log_write("haha\n");
		log_write("%d %s", 19, "heihehi");
	}
	return 0;
}
