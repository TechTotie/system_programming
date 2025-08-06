/*
 * @brief: environ variable and environ while using bash
 * In unix, all the environment variables can be accessed using
 * extern char ** environ;
 * which is a global variable, listing a list of environmental variables, which are key-value pairs.
 * Similarly the environment variables that are imported as soon as we run a bash/shell can be printed using
 * %printenv
 * This also returns a list of environment variables
 * If we want to access a particular environment variable, the shell provides it as a variable using the $ sign in front of the key
 * HOME can be accessed by usinng $HOME
 * PATH by $PATH
 * environment varaibles can be set by
 * export key=value
 * echo $key
 * This will be set on the local copy imported by the bash/shell.
 * The global copy needs to be set by editing bashrc
 *
 * Similarly a local copy of environment variables will be imported when in our c program by using the 
 * extern char **environ.
 * If we want to get any environment variable, we can use char * getenv("key");
 * If we want to set and unset any environment variable, we can use void setenv(char *key, const char *value, int overwrite); and void unset(char * key);
*/

#include <stdlib.h>
#include <stdio.h>

extern char **environ;

int main(int argc, char *argv[])
{
	/* Print all environment varaibles */

	char ** var;
	for(var = environ; *var != NULL; ++var) {
		printf("%s\n",*var);
	}

	/* get HOME */
	printf("HOME: %s\n", getenv("HOME"));

	printf("TSERVER_NAME: %s\n", getenv("TSERVER_NAME"));
	/* set new envvar TSERVER_NAME */
	setenv("TSERVER_NAME","example.com",1);
	printf("TSERVER_NAME: %s\n", getenv("TSERVER_NAME"));

	/* unset TSERVER_NAME */
	unsetenv("TSERVER_NAME");
	printf("TSERVER_NAME: %s\n", getenv("TSERVER_NAME"));

		
}
