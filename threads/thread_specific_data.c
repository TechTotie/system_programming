#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#define handle_error(msg) \
	do { perror(msg); exit(EXIT_FAILURE); } while(0);

#define handle_error_en(en, msg) \
	do { errno= en; perror(msg); exit(EXIT_FAILURE); } while(0);


/* Have a global key variable of type pthread_key_t */
pthread_key_t key;

/* Have a clean up function */
void clean(void *data)
{
	FILE * fp = (FILE *) data;
	printf("clean data : 0x%x\n",fp);

	int fd = -1;
	char buffer[1024];
	int ret = -1;

	/* Use either fread */
	/* ret = fread(buffer, 1024, sizeof(char), fp);*/
	
	/* Get file fd from FILE stream and use it to read */
	/* int fileno(FILE *stream) */
	fd = fileno(fp);
	if (fd == -1) {
		handle_error("fileno");
	}

	memset(buffer, 0, 1024);
	fseek(fp, 0, SEEK_SET);	
	ret = read(fd, buffer, 1024);
	//ret = fread(buffer, 1024, sizeof(*buffer), fp);
	printf("read ret: %d sizeof(*buffer): %ld\n", ret, sizeof(*buffer));

	if(ret <= 0) {
		handle_error("read");
	}
	printf("clean called for file with data\n");
	printf("%s\n", buffer);
	fflush(stdout);
	ret = fclose(fp);
	if(ret != 0) {
		handle_error("fclose");
	}
}

/* forward declaration */
void write_to_file(char * msg);

/* Have a thread-fn to set and get thread specific data */
void * thread_fn(void *id)
{
	char filename[1024];
	int index = *((int *)id);
	sprintf(filename,"thread_%d_%ld.logfile",index, pthread_self());
	fprintf(stderr, "Thread [%d] starting filename: %s\n", index,filename);

	FILE *fp = fopen(filename, "w+");
	printf("file fp: 0x%x\n",fp);
	if(fp == NULL) {
		handle_error("fopen");
	}

	/* set the thread specific data */
	printf("Set thread specific data : 0x%x\n",fp);
	pthread_setspecific(key, fp);

	char data[1024];
	sprintf(data,"Thread specific data for thread [%ld]\n",pthread_self());

	write_to_file(data);

	sleep(2);
}

void write_to_file(char * msg)
{
	FILE *fp = (FILE *)pthread_getspecific(key);
	if(fp != NULL)
		fprintf(fp,"%s",msg);
}

#define MAX 5
/* main program create key and register clean up function */
int main()
{
	pthread_t tid[MAX];
	int ret = -1;

	/* Create key and register clean up function before creation of threads */
	ret = pthread_key_create(&key, clean);
	if(ret != 0)
		handle_error_en(ret, "pthread_key_create");

	for(int i = 0; i < MAX; i++) {
		{
			int k = i;
			ret = pthread_create(&tid[i], NULL, thread_fn, (void *)&k);
		}
		printf("Thread[%d] created\n", i);
		if(ret != 0)
			handle_error_en(ret, "pthread_create");
	}
	for(int i = 0; i < MAX; i++) {
		ret = pthread_join(tid[i], NULL);
		if(ret != 0)
			handle_error_en(ret, "pthread_create");
	}
	return 0;
}
