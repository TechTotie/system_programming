# Temporary Files in Unix

A program might have the need to store some data temporarily for use until its execution.
If we use normal files for such requirement, it is responsibility of the creator of the file to delete the file after use.

Suppose, if we use normal files, instead of temporary files, then we might have to take care of the following
- There might be many instances of the same program running by the same or different user and each specific run of the program should have a particular temporary file associated with it to avoid collisions or data read/write/open/close.
- The file permissions should be such that only the program should be able to use the temporary file and should not be accessible by unauthorized user/programs.
- The names of the files should not be eqasily predictable as it can be explouted by external attacker.

In order to take care of above conditions,`Linux provides us a utilities to create temporary files.
All the temporary files are stored in `/tmp` directory in Linux.

Linux provides `mkstemp` and `tmpfile` api's to create temporary files.

Any file created in Linux filesystem, will have a directory entry made in the filesystem. If the file needs to be deleted from the linux filesystem, this file/directory entry should be removed from the filesystem, then only the file can be deleted.
Inorder to remove the directory entry from filesystem, linux provides ```unlink(const char *filename)``` api.
Using this api, Linux will remove the directory entry and then when all the fd's related to this file are closed, then the file is deleted from the filesystem. The file won't be deleted until all the associated fd's are closed. These fd's act as reference counters, so as long as there are open fd's related to file, the reference count for that file is greater than 0. Once the fd's are closed, the reference count decreases and the file will be deleted on ref_coiunt reaching 0.

There are 2 scenarios w.r.t., temporary files,
- The temporary file is used in a single process - better to use `tmpfile` as `tmpfile` is unlinked on creation
- The temporary file is used by 2 different processes - better to use `mkstemp` as needs to be manually unlinked.

Check the program [unix_temp_files.c](unix_temp_files.c) - See the documentation in this program.

```
#include <stdlib.h>
int mkstemp(const char * template)
```
- creates file with the name as per template, example
```
const char filename[] = "/tmp/file_name.XXXXXX"
```
the XXXXXX, (6 X's) will be replaced by mkstemp on creation of temporary file, that make file unique.

- returns the fd of the temporary file created.
- sets errno if unable to create temporary file
- sets the correct file permissions.
- You can use it with open, read, write, lseek, close api's.
- make sure you do lseek to correct position on the temporary file before reading it
There is a family of mkstemp api's.

```
#include <stdlib.h>
FILE * tmpfile(void);
```
- creates a temporary file with a unique name, no need to pass any argument
- unlinks the file as soon as it is created.
- since tmpfile() returns FILE* stream, fread, fwrite, fclose, fprintf, fseek etc., can be used to write to this temporary file.

Linux also provides mktemp, tempnam, tmpname api;s but they have security issues mentioned in the 3 points earlier.

**NOTE:**
Make sure that you unlink the temporary file you create, else it will fill up the `/tmp` folder in linux.

