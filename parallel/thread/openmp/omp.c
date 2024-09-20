#include <stdio.h>
#include <stdlib.h>

int main(){

#pragma omp parallel
{
	puts("HELLO");
	puts("WORLD");

}
	exit(0);
}


