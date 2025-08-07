/* Program to demonstrate error handling for system calls, Example: chown*/

#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
	const char path[] = "/random_file";
	uid_t uid = 1;
	gid_t gid = -1;

	int ret = chown(path, uid, gid);

	if(ret != 0) {
		/* First take copy of errno */
		int error_code = errno;
		/* if ret val != -1 then its a bug */
		assert(ret == -1);
		switch(error_code) {
			case EPERM:
			case EACCES:
			case ENOTDIR:
			case ENAMETOOLONG:
			case ENOENT:
			case EROFS:
				fprintf(stderr, "error changing ownership of file %s\n",strerror(error_code));
				break;
			case EFAULT:
				abort();
			case ENOMEM:
				fprintf(stderr,"no memory: %s\n",strerror(error_code));
				exit(1);
			default:
				abort();
		}
	}	else {
		printf("Owner of file %s changed to %d\n",path, uid);
	}
	
}


