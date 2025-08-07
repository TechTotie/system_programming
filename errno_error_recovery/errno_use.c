/*Program to demonstrate how to use errno */
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	int fd = open("/random_file",O_RDONLY);
	if(fd == -1) {
		/* First take backup of errno */
		int error_no = errno;
		perror("File open failed errormsg: ");
		fprintf(stderr, "File open failed %s\n",strerror(errno));
		abort();
	}
	close(fd);
}
