# Environment Variables

Unix uses environment variables for configuration of environment under which the programs will be run.
They can be categorized into
1. Static environment variables
2. Dynamic Environment variables

Static are the one's that are set once and will not be changed frequently.
Example: `PATH, HOME, USR` environment variables
We can access it using `$HOME`, home directory of $USR, `$USR`, current user of the system, `$PATH`, the path to be checked for any binaries.

Dynamic are the one's whose value will change dependending on the state of the system or state of the program being run.
Example: The return value of the program sun on bash can be accessed by env var `?`
Each time a program is run, the return value of the program will be set in env var `?` and we can access it using `$?`
If a program exits normally it returns 0, and non zero if it exits abnormally.
Example:
```
% ls
output -> lists the files and directories of $cwd
% echo $?
output -> 0
% ls ./invalid_file
output -> No such file or directory
% echo $?
output -> 2
```

Similar to `ls`, meaning list the files and directories of the argument passed to ls, which returns 0 on success and non zero on failure,
other user programs can also return 0 on success and non zero on failure.
We we do a return from main() function this return value is passed on to the parent which called the user program's main() function.
If we are using bash shell to execute user program binary, then this program will return the value to the bash shell.
This return value is set to `?` env var.

Environment variables are key-value pairs.
You can set and unset an environment variable using  
```
% export key=value
% echo $key
output => prints "value"
% unset key
% echo $key
output => does not print anything
```

Note that the environment variables set and unset using set and unset commands are local to the environment where they are set and unset.
It does not effect the global environment variables.

Example: If you use bash and set environment variable say `set my_var=value1` and execute `env` or `printenv` command in bash, you will see the list of all the environment variables
that are set. You can also see `$my_val` being listed in this list.
Now if you open another bash shell and then execute `printenv` or `env`, you can see that `$my_var` is not listed in the list of environment variables.
This is because, the setting and unsetting of environment variables is local to the environment in which they are set and unset and not global.

If you want to set/unset environment variables globally, we need to make changes in .bashrc file.


The Standard C Library provides API's to get and set environment variables.
```
#include <stdlib.h>

char ** environ; /* Provides list of all the environment variables */

char * getenv(const char *name); /* used to get the environment variable, returns NULL if there is no match */
int setenv(const char * name, const char * value, int overwrite); /* set an environment variables, sets errno on error, returns -1 on failure */
int unsetenv(const char * name); /* unset an environment varaible, sets errno on error, returns -1 on failure */

```
Refer [environ.c](environ.c)


