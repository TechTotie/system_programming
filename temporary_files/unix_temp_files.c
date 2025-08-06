/*
 * @brief: Unix tmp files using mkstemp and tmfile
 * There might be a requirement wherein you need to store data temporarily until your program executes and not needed later.
 * Or there might be a need wherein data needs to be passed temporarily between programs.
 * In such cases, you can create temporary files using unix system calls like open with read write permissions, and later unlink the file to remove it from file system.
 * But it causes many issues,
 * 1. What if there are multiple instances of your program being run either as same user or by different user. Then there will be name clashes.
 * 2. There might be a hacker using the same predictable name and he might be able to write into it between your program checking if the file exists till opening the file for writing.
 * 3. File permissions should be such that no external user will be able to access the temporary file.
 *
 * Using Unix provided mkstemp and tmpfile will address all the above mentioned issues, w.r.t., naming, scope, lifetime, usage and security.
 * If you need to pass the temporary file to another program use mkstemp.
 * If you just need to temporarily create file for a single program and do system calls like read, write then use tmpfile call.
 * Unix also provides mktmp, tmpname, tempnam, but these calls have security issues, so better to use mkstemp and tmpfile.
 *
 * In Unix, whenever a file is created, it's entry is made into directory system. (along with creation of a file in the file system).
 * Each file created has a reference count (ref_count) of the file descriptors (fd's) related to that file.
 * The file will not be removed from the file system until all the fd's related to that file are closed and it's entry in the directory system is removed.
 * Unix provides a way where you can remove the file from filesystem by removing it's entry from the directory system, still making the file available until all it's fd's are closed.
 * This can be done by calling unlink(filename), where the entry for this file is removed from the directory system.
 * This can be done on any file.
 *
 * If we create temporary file using mkstemp, we have to unlink it explicitly,
 * But creation of temporary file using tmpfile will unlink it automatically.
 * The mkstemp will take a template as an argument, which it uses to create unpredicatble random filenames
 * int mkstemp(char * template);
 * The template is in the form "XXXXXX", (6 capital X's).
 * Usually it is like filename.template
 * mkstemp will return file descriptor which can be used for read, write system calls.
 * 
 * tmpfile will create a temporary file and unlinks the file as soon as it is created.
 * So this file gets deleted from file system as soon as the program that created temporary file using tmpfile exits.
 * FILE * tmpfile(void);
 * It returns the File stream that can be used with fopen, fprintf and fclose
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

/* API to create temporary file and write data to it. The caller should close the fd after usage of temporary file */
typedef int temp_file_fd;

temp_file_fd write_to_temp_file(const char * buffer, size_t len)
{
	/* Create temporary file using mkstemp */
	char filename[] = "/tmp/temp_file.XXXXXX";
	int fd = mkstemp(filename);
	if(fd == -1) {
		perror("mkstemp");
	}

	/* Unlink the file if not passed across programs */
	unlink(filename);

	/* Write data  to file - first write length of data and then data into temporary file */
	printf("buffer[%d]: %s\n", len, buffer);
	int ret = write(fd, &len, sizeof(len));

	if(ret == -1) {
		perror("write");
	}

	ret = write(fd, buffer, len);
	if(ret == -1) {
		perror("write");
	}
	
	/* Return the temporary file fd */
	return fd;
}

/* API to read from temporary file */

char * read_from_temp_file(temp_file_fd fd)
{
	size_t len = 0;
	if(fd == -1) {
		perror("invalid fd");
	}

	/* Seek the file posn to its initial position as it may be at any position */
  /* Use lseek as it is based on fd not on FILE * stream */
	lseek(fd, 0, SEEK_SET);
	
	/* Read len from temporary fd */
	int ret = read(fd, &len, sizeof(len));

	if(ret == -1) {
		perror("read");
	}

	/* Allocate memory for reading data from temp file. */
  /* This memory should be released/freed by caller */
	char * buffer = (char *) malloc(len * sizeof(char));

	if(buffer == NULL) {
		perror("malloc");
	}

	ret = read(fd, buffer, len);
	if(ret == -1) {
		perror("read");
	}

	return buffer;
}

/* Driver program */
int main(int argc, char * argv[])
{
	char str[] = "This is a temporary string";
	int fd = write_to_temp_file((const char *)str, strlen(str));

	if(fd == -1) {
		perror("Unable to write to temp file");
	}

	printf("Reading from temporary file: %s\n", read_from_temp_file(fd));

	close(fd);

	return 0;
}
