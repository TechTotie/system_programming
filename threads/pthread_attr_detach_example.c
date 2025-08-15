/* Program to demonstrate the detached threads using thread attributes */

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

void * thread_function(void *data)
{
	fprintf(stderr, "In thread [%ld] sleeping for 2s so that main thread exists\n",pthread_self());
	sleep(2);
	fprintf(stderr, "This should not be printed");
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;
	
	/* Initialize the attribute */
	pthread_attr_init(&attr);
	
	/* set the detached state for the thread to be created */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	/* create the thread passign attr as argument */
	int ret = pthread_create(&tid, &attr, thread_function, NULL);
	if(ret != 0) {
		fprintf(stderr, "pthread_create error %s\n", strerror(ret));
		exit(1);
	}
	sleep(1);

	/* Trying to join this thread should return error */
	if(!pthread_equal(pthread_self(), tid)) {
		int ret = pthread_join(tid, NULL);
		printf("ret: %d\n",ret);
		if(ret != 0) {
			fprintf(stderr, "pthread_join error %s\n", strerror(ret));
			exit(1);
		}
	}
	printf("Here\n");
	return 0;
}
