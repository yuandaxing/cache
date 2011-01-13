#include "detach.h"
#include <unistd.h>

int main()
{
	detach();
	while(1) sleep(1);
	
	return 0;
}
