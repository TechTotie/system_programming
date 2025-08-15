/* Program to demonstrate clean-up handlers */

#include <pthread.h>
#include <stdio.h>

/* Clean up forward declaration */
void cleanup(void * data);

/* initialization */
void * allocate(size_t size)
{
	if(size > 0)
		return (void *)malloc(sizeof(size));
	else
		return NULL;
}

/* clean up function */
void cleanup(void * data)
{
	if(data != NULL)
		free(data);
}

/* Thread function */
void * thread_fn(void * data)
{
	/* Thread specific buffer */
	void * temp_buffer = allocate(10);

	/* register cleanup function */
	pthread_cleanup_push(cleanup, temp_buffer);

	// do some work

	/* call clean up and deregister the cleanup function */
	pthread_cleanup_pop(1);
}
