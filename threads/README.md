# Threads

Whenever we run a program in linux, it creates a running process. A running process internally has its own main thread on which it executes. Threads are granular level schedulable entities that are present in a process. They are similar to processes except the fact that they share the same virtual memory, open file descriptors etc., of the process of which they are part of. But on the other hand the processes have their own copy of virtual memory, file descriptors etc., which they create a copy from the parent process.

GNU/Linux provides `pthread` library and header file which has all the api's related to creation and managing the threads.

** Create a Thread **

A thread can be created using `pthread_create` call, which takes in 2 parameters
- an integer that represents the thread id, of type `pthread_t`
- a `pthread_attr_t` parameter, that has thread atttributesi, if NULL created with default attributes
- a function that the thread should execute of type `void* (*fn)(void *)`
- the parameters to the thread function of type `void *`, if NULL means no arguments to function

The `pthread_create` creates a thread and runs the function that is passed as argument while creation of the thread.

See [example](pthread_create_example.c)
From the example, you can see that the `pthread_create` function is called with NULL arguments
```
pthread_t tid;
int ret = pthread_create(&tid, NULL, printxs, NULL);
```
The second parameter is NULL, which indicates that default attributes are considered for thread creation.
The return value of pthread_create is 0 on success and `errno` on failure, which can be directly used to print error message.
To compile the program we need to link it with pthread library, `-lpthread`.

The threads can exit in 2 ways
- Normal exit by return statement
- pthread_exit, that exists the thread from which it is called.

Here we have considered an example, where the thread runs infinitely because fo `while(1)`.

What if the thread executes a function that returns after doing certain job.
Let us take an example.
```
void * compute(void * unused)
{
	// Do dome computations
	for(int i = 0 ; i < 100; i++)
		fputc('x',stderr);
	return NULL;
}
int main()
{
	pthread_t tid;
	int ret = pthread_create(&tid, NULL, compute, NULL);
	if (ret == -1)
		perror("pthread_create failed");
	for(int i = 0; i < 10; i++)
		fputc('o',stderr);
	return 0;
}
```
Now, if you execute this program and see the output,
you can see that the output may not have any x's printed or only some number of x's printed. But all the number of o's are printed.

This is based on 2 factors
- First how the threads are scheduled
- Second whether the parent/main thread will wait for all the other threads to finish computation.

We do not have any control over how the threads are scehduled and every run of the program may produce different output.
If the main thread completes execution then the entire program will stop execution.
And hence we cannot get the output of the child thread.

** Join a thread **
Now, in order to synchronize threads, we need the main thread to wait until its child threads complete the execution.
So, ptherad library provides a similar call as `wait` that we used in the parent process to wait for the child process to complete.
This is `pthread_join()` call, which expects the thread id to wait for completion and also collects the return value of the thread.
```
int pthread_join(pthread_t tid, void ** retval);
```
The pthread_join() returns 0 on success and errno on error.

It should be taken care that we dont call wait in the same thread for the same thread. This will create deadlock.
ptherad library provides 2 funtions in order to check if we are calling pthread_join on self,
they are
- `pthread_self`, which gives the thread id, in the thread in which this function is called
- `pthread_equal`, which will allow to compare the thread id's.
We can compare the thread id's
So,
```
if(!pthread_equal(pthread_self(), other_thread)) {
	int ret = pthread_join(other_thread, NULL); // pass Null if you dont want to collect the return values
}
```

What happens if we try to join the thread to itself,
`pthread_join()` will return errcode `EDEADLK` and it's not allowed.

** Arguments and Return Values from Threads **
We can pass arguments to threads and also collect return values from thread.
The collection of return values from thread can be done using join.
Passing of arguments can be done by casting the arguments to `void *` and passing them to functiom.
Note if you need to pass more than a single argument, it is better to group them in strcuture and then pass the pointer to the object of this structure.
Let us rewrite the above example
[pthread_join_example.c](pthread_join_example.c)
```
/* say we want the thread to execute a function which prints a given character, given number of times to stderr */
/* for this we need to pass the character to be printed and the count of how many times the character needs to be printed to the function */
/* We need to group them in structure */
/* Lets say this function also returns the number of times the thread has actually printed the character onto screen to avoid any unforseen exit scenarios */

struct data {
	char c,
	int count;
};
void * printchar(void * input)
{
	int count = 0;
	struct *ptr_data = (struct data *)input;
	if(ptr_data != NULL)
		for(int i = 0; i < ptr_data->count; i++) {
			fputc(c, stderr);
			++count;
		}
	return (void *)&count;
}
int main()
{
	pthread_t t1,t2;
	data d1, d2;
	int c1, c2;

	d1.c = 'x'
	d1.count = 10;
	d2.c = 'o';
	d2.count = 10;

	int ret = pthread_create(t1, NULL, printchar, &d1);
	if(ret != 0) {
		perror("ptherad_create failed");
		exit(1);
	}
	ret = pthread_create(t2, NULL, printchar, &d2);
	if(ret != 0) {
		perror("ptherad_create failed");
		exit(1);
	}

	/* VERY IMPORTANT IS TO JOIN THESE THREADS */
	pthread_join(t1, (void *)&c1);
	pthread_join(t2, (void *)&c2);

	printf("thread 1 printed : %d times\n", c1);
	printf("thread 2 printed : %d times\n", c2);
}
```

You can also check the other program that returns the nth prime number,
[compute_prime_thread.c](compute_prime_thread.c).
Please note how the values are returned. 
```
int prime;
return (void*)prime;
```
Don't do `(void *)&prime` as this returns the location of prime, which is a local variable allocated on stack and will cause `Segmenation fault`.


** NOTE ** that the threads can be joined only if the threads are joinable.
If the threads are in the detached state, they cannot be joined.

The threads can be created in 2 states
- Joinable state
- Detached state.

If the threads are created as joinable threads, then the threads can be joined.

If the threads are created as detachable threads, then the threads cannot be joined.
The detached threads are similar to that of the `zombie processes`, where Linux takes up the responsibility of cleaning up these threads.

The threads can be created as joinable or detached by setting the attrribute
 



