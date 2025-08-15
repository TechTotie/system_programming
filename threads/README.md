# Threads

Whenever we run a program in linux, it creates a running process. A running process internally has its own main thread on which it executes. Threads are granular level schedulable entities that are present in a process. They are similar to processes except the fact that they share the same virtual memory, open file descriptors etc., of the process of which they are part of. But on the other hand the processes have their own copy of virtual memory, file descriptors etc., which they create a copy from the parent process.

GNU/Linux provides `pthread` library and header file which has all the api's related to creation and managing the threads.

** NOTE **
Try to avoid using errno values when using pthread library and api's as most of the pthread api's will return the error number as return values and do not set the global errno variable.

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

** Thread Attributes and Detached Threads **
The threads can be created in 2 states
- Joinable state
- Detached state.

By defaul the threads are created in joinable state.
If the threads are created as joinable threads, then the threads can be joined.

If the threads are created as detachable threads, then the threads cannot be joined.
The detached threads are similar to that of the `zombie processes`, where Linux takes up the responsibility of cleaning up these threads.
On the other hand if the thread is joinable, then it is the responsibility of the parent thread to clear the exit status of the thread by joining this thread to itself.
A thread can be joined in any other thread. The `pthread_join` will also collect the return value of the thread.

We can create a thread in detached state by using thread attributes.

The thread attributes are used while creation of the thread as a parameter to `pthread_create`.

In order to create the thread attributes
- We need to declare a varibale of type `pthread_attr_t`
- We need to initialize it using `ptherad_attr_init` and pass the variable as an argument to this function
- We can modify the attributes accordingly
- We can use in thread creation function
- We can destroy the attribute using `pthread_attr_destroy`

In the below example, we create a detachable thread using `pthread attributes`
```
void * thread_fn(void *)
{
 ...
	// Do something
	...
}

int main()
{
	pthread_t tid;
	pthread_attr_t attr;

	/* Initialize the pthread attribute */
	ptherad_attr_init(&attr);

	/* set the thread detach state to detached */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

	/* create pthread using attribute */
	/* Pass the attribute as argument to pthread_create */
	int ret = pthread_create(&tid, &attr, thread_fn, NULL);
	if(ret != 0) {
		/* Return value has the error number */
		/* strerror(error_number) will return a fixed error string for that error number, please cross check for the error in man page of the pthread function that returned this error */
		fprintf(stderr,"Error: %s", strerror(ret));
		exit(1);
	}

	/* Destroy the attribute */
	pthread_destroy(&attr);

	/* Try joining this thread, it will return EINVAL - Invalid argument as the thread id is argument which is a detached thread and cannot be joined */
	{
		int ret = pthread_join(tid, NULL);
		if(ret != 0) {
			fprintf(stderr, "Error %s\n", strerror(ret));
			exit(1);
		}
	}
}
```


The thread can also be detached by using `pthread_detach(pthread_t tid)`.
Once the thread is detached, its resources are collected by the system after the thread exits. It cannot be joined again and there is no need for another thread to collect its resources.

** Thread Cancellation **

Normally a thread exits using `pthread_exit` or completes execution and returns. 
In some cases, it may be needed that a thread needs another thread to exit or it needs to cancel the execution of another thread.
We can use the `pthread_cancel(ptherad_t tid)` api to cancel the execution of another thread.

A thread can be
- Asynchronously cancelled - cancelled anytime during execution
- Syncronously cancelled - cancellation requests are queued up and processed at particular `cancellation points`
- Uncancellable - cancellation requests are just ignored.

A thread's cancellation depends on the following factors
- Cancellation State - Enabled/Disabled
- Cancellation Type - Synchronous/Asynchronous

```
int pthread_setcancelstate(int state, int *oldstate);
int pthread_setcanceltype(int type, int *oldtype);
```

The following exvents take take on thread cancellation
- Cleanup function is called, 
		-	thread specific data is deallocated
- Clean up function is popped
- Thread is termincated by calling `pthread_exit(void ** retval)`

There might be data leaks when the thread is cancelled asynchronously, that is why pthread library allows to register(push) clean-up functions, which will be called and thread specific data can be deallocated on thread cancellation/termination.

But during normal termination of thread, the user has to call the clean-up function to deallocate the thread-specific data and deregister(pop) the clean-up handler.

We can also enable and disable the thread cancellation for any atomic operations in asynchronuous cancellation mode too.
```
void * thread_fn(void *)
{
	// Do Something

	/* Atomic Operations start */
	/* Disable thread cancellation */
	int oldstate = 0;
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, oldstate);

	// Perform the atomic operations

	/* Atomic operations end */
	/* Enable thread cancellation or reset to old state */
	pthread_setcancelstate(oldstate, NULL);

	// Do something else

	return;
}
```

If we perform cancel when the thread is in non-cancellable state, then the cancellation request will be queued, and executed once the is cancellable again.

For synchronous cancellation, we need to have `cancellation points` at various steps in the thread function, so that the thread can be cancelled at these cancellation points. These can be set by either calling
- `pthread_testcancel` or 
- some system functions that are cancellable, listed in `man 7 pthreads`

To make a thread synchronously cancellable, use
```
void * thread_fn(void *)
{
	int old_type;
	pthread_setcanceltype(PTHREAD_CANCEL_DEFERRED, &old_type);
}
```
To make it asynchronous, use
`pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &old_type)`.


** Thread Specific Data **

All the threads share data, so any changes in global or static variables, will be reflected across all the threads.
In order to have thread specific data, GNU/Linux provides the following types and api's
- you need to create a key using `pthread_key_create`. Create a global key of type `pthread_key_t` which is shared across all the threads.
- you can initilaize this key with a `clean-up` function that will be called on termination of thread. Note that the clean-up function will be called directly with the `viod *` pointer to the thread specific data, that can be directly de-allocated. There is no need to call `pthread_getspecific` with the key in clean-up function.
- you need to set the value of type `void *` for this key inside the thread using `pthread_setspecific(pthread_key_t key, void* data)`
- you can access this thread specific data using `void * data = pthread_get_spcific(pthread_key_t key)`

Consider a use case, where each thread needs to have its own log file to record its actions
```
/* Error function */
#define handle_error(en, msg) \
	do { errno = en; perror(en); }while(0);

/* Have a global key across all threads */

pthread_key_t key;

/* Have a clean up function that is to be registered for clean up during thread termination */
void close(void * data)
{
	FILE * filename = (FILE *)data;
	close(filename);
}


/* set the thread specific data and use it across other functions called using thread_fn */
void * thread_fn(void *data)
{
	char filename[1024];
	fprintf(filename, "thread_%d.logfile",pthread_self());

	/* set the thread specific data which is the filename */
	pthread_setspecific(key, filename);

	char data[1024];
	fprintf(data, "Thread data for thread %d \n",pthread_self());
	write_to_file(data);

}

void write_to_file(char * msg)
{
	FILE * filename = pthread_getspecific(key);```
	fprintf(filename,"%s",msg);
}

/* main fn has to create pthread_key_create */
int main()
{
	/* Create the key and register the clean up function */

	pthread_create_key(&key, close);

	pthread_t tid[5];

	for(int i = 0; i < 5; i++) {
		int ret = pthread_create(&tid[i], NULL, thread_fn, NULL);

		if(ret != 0) {
			handle_error(ret, "pthread_create");
		}
		pthread_join(tid, NULL);
	}
}
```


See the example [thread_specific_data.c](thread_specific_data.c) for more detailed example.

In the example we can see that we have used 2 ways to read from the file.
- read using fd, which is obtained by `int fileno(FILE *stream)`
- read using `fread(FILE * stream, int no_of_elements_of_array, int size_of_each_array_element, void * buffer);`
- Make sure you point to the first position of the file using `fseek(FILE *stream, 0L, SEEK_SET)` to make it point to first character of the file.

We can also note the usage of
```
for (int i = 0; i < MAX; i++) {
	{
		int k = i;
		ret = pthread_create(&tid, NULL, thread_fn, &k);
	}
}
```
instead of passing `&i` directly, this is because the thread is created and immedicately i is incremented and since we are passing the address of i, the value of i will be updated, which will also be reflected in the thread.

This type of cleanup will be done only when the thread terminates using `pthread_exit()` or if a thread is cancelled by another thread and specially in case of thread specific data. How will the clean up be done for a thread that exits normally and if it does not have any thread specific data.

GNU/Linux provides *** Clean Up Handlers *** for such cases.
- We need to register the clean-up handler with `pthread_cleanup_push(void (*clean_up_func)(void*), void * data)`, where we need to provide the clean-up function of type `void cleanup(void *)` and data of type `void *`.
- The function can be deregistered by calling `pthread_cleanup_pop(int num)`, where a non zero number means that the clean up function is called before the cleaan up function is deregistered.

 Refer the code in [pthread_cleanup_handler.c](pthread_cleanup_handler.c)


The same clean up in C++ needs to be done such that the pthread_exit should be called from a higher level exception rather than direct call.
This is because C++ does not guarantee calling destructors of automatic variables which are declared at beginning, when the thread exits directly using `pthread_exit`. But when we use exceptions, there will be stack unwinding and then we can call pthread_exit in the exception.

```cpp
#include <pthread.h>

class ThreadExitException {
		private:
			void * thread_return_value;
		public:
			ThreadExitException(void * ret_val)
			  : thread_return_value{ret_val} {
			}
			void* DoExit() {
				pthread_exit(thread_return_value);
			}
};

void * thread_fn(void * data)
{
	try {
		// Due to some reasons need to exit
		throw ThreadExitException(NULL); // or any return value
	} catch(ThreadExitException &e) {
		e.DoExit();
	}
}
```
