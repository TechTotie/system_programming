/* Program to show error handling while reading a file */
/*
 Let us consider the following steps in order to read a file
1. Allocate a buffer
2. Open a file
3. Read the contents of the file onto buffer.
4. close the buffer
5. Return the buffer

Note that step 1 can be done after step 2 but here we are doing in this order to show abotu clean up
*/
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>

char * read_from_file(const char * path, size_t length)
{
	/* Allocate the buffer */
	char * buffer = (char *)malloc(length * sizof(char));
	if(NULL == buffer) {
		/* no clean up required as first step is null */
		return NULL;
	}
	/* open file */
	int fd = open(path,O_RDONLY);
	if(-1 == fd) {
		/* perform cleanup of buffer */
		int error_code = errno;
		free(buffer);
		fprintf(stderr,"Error opening file : %s\n",strerror(error_code));
		return NULL;
	}

	/* read from file */
	int len = read(fd, buffer, length);
	if(-1 == len) {
		/* perform clean up of buffer and file fd */
		int error_code = errno;
		fprintf(stderr,"Unable to read from file %s\n", strerror(error_code));
		free(buffer);
		close(fd);
		assert(len != -1);
	}

	/* Since everything went smooth close the file */
	close(fd);

	/* return the buffer */
	return buffer;
}

/*
* Note: Instead you could have a cleanup api, where you can pass the resources and you can at each step of failure call clean up
*/
