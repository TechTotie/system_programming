/* Program to compute nth prime number in a separate thread */
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

void * compute_prime(void * arg)
{
	int prime = -1;
	int n = *(int *)arg;

	printf("input argument: %d\n",n);

	int count = 0, i = 2;

	if(n <= 0) {
		// Dont return anything
	} else if (n == 1) {
		prime = 2;
	} else {
		while(1) {
			if(count == n) {
				prime = --i;
				break;
			}
			else {
				int k = 2;
				for(; k < i; k++)
					if(i%k == 0)
						// Not prime
						break;

				if(i == k)
					// prime
					++count;

			}
			++i;
		}
	}

	if(prime != -1)
		return (void *)prime;
	else
		return NULL;
}

int main(int argc, char *argv[])
{
	int n = atoi(argv[1]);
	int prime = -1;
	pthread_t tid = -1;
	int ret = pthread_create(&tid, NULL, compute_prime, &n);
	if(ret != 0) {
		perror("pthread create");
		abort();
	}
	ret = pthread_join(tid, (void *)&prime);
	printf("%dth prime number is : %d\n", n, prime);
}
