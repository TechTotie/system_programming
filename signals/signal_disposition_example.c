/* Program to demonstrate signal disposition */
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

sig_atomic_t signal_count = 0;

void signal_handler(int signum)
{
	++signal_count;
}

int main()
{
	struct sigaction sa;

	/* Reset the complete sigaction structure */
	memset(&sa, sizeof(sa),0);

	sa.sa_handler = signal_handler;

	sigaction(SIGUSR1, &sa, NULL);

	sleep(60);

	printf("Number of times SIGUSR1 signal occured : %d\n", signal_count);

	return 0;
}

