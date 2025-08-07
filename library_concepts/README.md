# Libraries

Most of the user programs or applications are linked with the libraries depending on the application.
For example, if it is a GUI application, a windowing system library will be linked, similarly for a database application, a database library will be linked.

By default, the C compiler, gcc will link `libc` with all the C applications.
The C++ compiler, g++ will link `libstdc++` with all the C++ applications.

Libraries are created by linking/combining the object files. This can be done in 2 ways and so there are 2 kinds of libraries
1. static libraries - Created as an archive of different object files.
2. dynamic libraries - Created as a single large shared object file from various input object files.

## Static Libraries

Static libraries are created by using the archive `ar` command/tool. They are called `archive(s)` in linux and `.LIB` in Windows.  
Suppose we have 2 src files,
```
test1.c
void f()
{
 /* some implementation */
}
void g()
{
 /* some implementation */
}

test2.c
void f()
{
	/* some implementation */
}
void g()
{
 /* some implementation */
}
```
If we want to create a static library called `libtest`, then
```
% gcc -c test1.c
% gcc -c test2.c

** % ar cr -o libtest.a test1.o test2.o **
```
Static libraries have extension `.a` meaning archive. The archive is just a collection of object files.
All the libraries have a naming convention that their name should start with `lib`.
We use the `ar`, archive tool with `cr` flag, which tell the archive tool to create an archive.

Please note that we have the same implementation in both test1.c and test2.c. But still the archive tool will not crib anything about repeated functions, but it just creates the archive with both these object files.

Now, if we have a driver program
```
driver.c
void f();
void g();

int main()
{
	f();
	g();
}
```
Now if we compile and link the driver program with the archive
```
% gcc -c driver.c
% gcc -o driver driver.o -L . -ltest
```
the compilation and linking will go through without any errors.

This is because the linker will load the required object files when creating an executable. So, since we have created an archive of  2 object files having the same functions, it just loads the first object file from the archive while creating the executable.
Note that this only works with archives, but not individual linking, say instead of `% gcc -o driver driver.o -L. -ltest`, you had directly linked test1.o and test2.o as `% gcc -o driver driver.o test1.o test2.o`, then you would receive `multiple definition error`. This is because linker links all the object files that is given as input. But when it comes to libraries, it tries to find the dependency and loads only the required portion. The difference is that in static library, it extracts the object file from the archive/static library that fulfils the dependency(linker stops loading other object files from the archive as soon as dependencies are met) and makes it part of the executable.
The linker is designed such a way that it will start parsing the input files from left to right and as it parses each input file, it creates the list of the unresolved symbol and tries to resolve them with next input files given in the command.
Suppose let us say our archive file had dependecny on another function or variable that is present in another object file or another library then once the linker parses the archive file, it identifies the dependency and searches for it in the next input file given in the command line.
**NOTE** Linker parses all the files given as input and does not skip any.

We have given the options
- `-L` to indicate the folder from which the archive has to be loaded.
- `-l` to indicate the library name that is to be linked. `-l<libraryname>` without the `lib` for both static and shared libraries.

What if had linked the above program in a reverse order
```
% gcc -o driver -L . -ltest driver.o
```
This would produce
*unresolved symbols for function f and g referenced in main error*
As explained earlier the linker needs inputs in orderly fashion in which dependencies are resolved one after other and not created one after another.

Sometimes it may so happen that there might be cyclic dependency of a static library with another static library, in that case link one of the library twice.

```
% gcc -o app app.o -L . -lfoo -lbar -lfoo
```


## Dynamic Libraries

These are similar to static libraries but they are just one big single shared object files. They are also called as `shared objects`.On windows they are called `Dynamic Link Libraries`.
Since they are shared, they have **reference counts** associated with them.
They are shared across multiple programs. Meaning a shared library can be used by multiple programs, but there is a single instance of a shared library loaded into memory. This is unnloaded from memory only when reference count is 0.
Unlike static libraries, shared objects are not part of the executable.

Since they are not part of executable, they are not loaded along with the executable. But when the extecutable will refer or call any function that is present in the shared object, only that function from the large shared single object is loaded into the memory at any address. So in order to make the code independent of position in memory we use the option `-fPIC` with gcc, while creating the shared object. `PIC` stands for **Position Independent Code** - meaning the functions in the shared object may be loaded at different addresses in different porgrams.

So, if we want to create shared object of above test1.c and test2.c.
```
% gcc -c -fPIC test1.c
% gcc -c -fPIC test2.c
% gcc -shared -fPIC -o libtest.so test1.o test2.o
```

Note that we cannot create a shared object from test1.c and test2.c as they both have the same functions.
Since shared objects are single large objects, it does not allow to have multiple definitions of the same function.

Now suppose we have both libtest.a and libtest.so present in the same directory, by default the shared object will be chose by compiler for linking and producing executable.
But if we need to use the static library, we need to inform the compiler by providing `-static`
```
% gcc -static -o driver driver.o -L . -ltest
```

When the shared libraries are loaded, they inturn load their dependencies automatically, if they were mentioned or linked while creating the shared library.
We can get to know the dependencies of the shared libraries by using the `ldd` command.
```
% ldd libtest.so
```
output-> `/lib/libc` -> standard C library and ** `/lib/ld-linux.so` ** -> part of GNU Linux linking system.

While executing the binary, the loader will load the libraries by default from `/lib` and `/usr/lib`.
If we need to load it from any other location we need to provide it while linking or loading

```
% gcc -o driver -L . -ltest -Wl,rpath, /usr/local/lib/
```

The options to linker `-Wl,rpath` will ask the loader to first look at the shared objects in `/usr/local/lib` folder following other by default folders like `/lib and /usr/lib`.

Similarly, we have another option for making the loader to look for sahred objects in specific folders before default folders.
This is by setting `LD_LIBRARY_PATH`, environment variable, either locally or globally.
`LD_LIBRARY_PATH` is similar to `PATH`, which is a `:` separated string. Example: If we set
```
export LD_LIBRARY_PATH=/opt/lib:$LD_LIBRARY_PATH
```
This will prefix `/opt/lib` to LD_LIBRARY_PATH and start looking for shared objects in the order mentioned in the LD_LIBRARY_PATH and then default folders.

** NOTE **
Once the linker/loader finds the library which it is looking for in the path mentioned by LD_LIBRARY_PATH, then it will stop parsing the paths in LD_LIBRARY_PATH. 

**NOTE**
Please note that C++ compiler, g++ will mangle with the name provided, but C compiler, gcc will not mangle with the name.
So when creating a shared object with C++ functions it's better to use `extern "C"`
```
extern "C" void f();
```

## Dynamic Loading and Unloading

Standard C Library provides a way to load a shared object or dynamic link library during runtime, instead of linking it during compiletime.

```
#include <dlfcn.h>
void * dlopen(const char *pathname, int flags);
void * dlsym(void * handle, const char *fn_name);
void * dlsym(void * handle, const char *fn_name, int version);
int dlclose(void * handle);
```

The above mentioned function can be used to open a shared object, load a function from shared object, call the function and close the shared object. The dlopen() and dlcose() will increment and decrement the reference count of shared object. If dlopen() will open the shared object if ref_count is 0, if ref_count>0, means library is already loaded, and dlclose() will close the shared object when the ref_count is 0.

```
#include <dlfcn.h>

int main()
{
	void * handle = dlopen("libtest.so", RTLD_LAZY);
	void(*fn)() = dlsym(handle, "f");
	fn();
	dlclose(handle);
}
```
