/* Program to demonstrate pthread_join */
/* say we want the thread to execute a function which prints a given character, given number of times to stderr */
/* for this we need to pass the character to be printed and the count of how many times the character needs to be printed to the function */
/* We need to group them in structure */
/* Lets say this function also returns the number of times the thread has actually printed the character onto screen to avoid any unforseen exit scenarios */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

struct data {
	char c;
	int count;
};
void * printchar(void * input)
{
	int count = 0;
	struct data *ptr_data = (struct data *)input;
	if(ptr_data != NULL)
		for(int i = 0; i < ptr_data->count; i++) {
			fputc(ptr_data->c, stderr);
			++count;
		}
	return (void *)count;
}
int main()
{
	pthread_t t1,t2;
	struct data d1, d2;
	int c1, c2;

	d1.c = 'x';
	d1.count = 10;
	d2.c = 'o';
	d2.count = 10;

	int ret = pthread_create(&t1, NULL, printchar, &d1);
	if(ret != 0) {
		perror("ptherad_create failed");
		exit(1);
	}
	ret = pthread_create(&t2, NULL, printchar, &d2);
	if(ret != 0) {
		perror("ptherad_create failed");
		exit(1);
	}

	/* VERY IMPORTANT IS TO JOIN THESE THREADS */
	pthread_join(t1, (void *)&c1);
	pthread_join(t2, (void *)&c2);

	printf("thread 1 printed : %d times\n", c1);
	printf("thread 2 printed : %d times\n", c2);
	return 0;
}
