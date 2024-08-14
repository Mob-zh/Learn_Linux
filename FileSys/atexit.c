#include <stdio.h>
#include <stdlib.h>

static void f1(){
	puts("f1()");
}

static void f2(){
        puts("f2()");
}

static void f3(){
	puts("f3()");
}

int main(){

	puts("Begin\n");

	atexit(f1);
	atexit(f2);
	atexit(f3);

	puts("End\n");
	exit(0);

}



