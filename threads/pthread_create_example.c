/* Program to demonstrate creattion of thread using pthread_create */

#include <pthread.h>
#include <stdio.h>
#include <string.h>

void* print_x_s(void * arguments)
{
	while(1) {
		fputc('x',stderr);
	}
	return NULL;
}

int main()
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, print_x_s, NULL);
	if(ret == -1) {
		fprintf(stderr, "ptherad_create failed: %s", strerror(ret));
	}
	while(1) {
		fputc('o',stderr);
	}
	return 0;
}
