#include <stdio.h>
#include <stdlib.h>
#include <time.h>
int main(){
	
	int64_t count;
	time_t end = time(NULL) + 5;
	
	while(time(NULL) <= end)
		count ++;

	printf("%lld\n",count);

	exit(0);

}


