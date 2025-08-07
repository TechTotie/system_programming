# Example program to depict usage of gcc, g++, gdb and makefile

The reciporcal.hpp has the declaration of the reciprocal function.  
This can be called from c program hence we use
```
#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif
```

The macro `__cplusplus` is enabled by the compiler when we are compiling \*.cxx files. 
It tells the compiler to enable anything mentioned in between the macro 
`#ifdef __cplusplus` and `#endif`

Adding `extern "C" {` will treat the code following this as C code until it encounters corresponding `}`.


### assert

You can see that I have also add the `assert(i != 0)` to avoid any errors/exceptions/faults caused by "division by 0" 
If the condition in the assert is false, it fails and aborts the execution. 
This happens when i = 0 and hence the program will not proceed.  

Note that the assert is a macro that will be replaced with a its code during preprocessing stage and it has its own set of overhead during execution. 
This overhead can be skipped by using NDEBUG compiler flag during compilation. 
Using this flag will skip assert commands.  

**NOTE:** So, make sure that you do not call any function calls, or do any assignment operations inside assert macro, as there is a chance of program being compiled with NDEBUG and these statements
will not be exexcuted. 

Exmaple: Suppose you want to check the return value from a function, then DON'T do it inside assert
```
	for(int i = 0; i < 100; i++) {
		
		assert( false != dosomething());	/* => If compiled with NDEBUG, dosomething() will not be called at all */
		/* OR */
		int k = 0;
		assert (-1 != (k=dosomething()));	/* => If compiled with NDEBUG, dosomething() return value will not be assigned to k */
		++k;
	}


	/* Instead you can do */


	for(int i = 0; i < 100; i++) {
		bool ret = dosomething();	/* => dosomething() will be called even if compiled with NDEBUG */
		assert(ret != false);	/* => Only this check will be skipped if compiled with NDEBUG */
		OR
		int k = 0;
		k = dosomething();	/* => dosomething() will be called and k will be assigned with return value from dosomething(), even if compiled with NDEBUG */
		assert(k != -1);	/* => Only this check will be skipped if compiled with NDEBUG */
		++k;
	}
```

It is good to use assert for checking a pointer is assigned to null or not.
```
assert(pointer != NULL);
```
Dont use assert directly to check for user inputs and exit the program. Its not a good way to program.


**NOTE:**
It is **ALWAYS BETTER** to use asserts to avoid bugs at later point of time. But use it only for condition checks, not combined with any other operations.


## Compilation

Now, in order to compile the above program, we can use gcc to compile C program and g++ to compile the C++ programs. Else we can use g++ entirely.  
Note that there are 2 implementation files, main.c and reciprocal.cpp, and one single header file reciprocal.hpp.
The implementation of reciporcal is in reciprocal.cpp and this function is called by main() function.  
We need to compile both the programs to generate object files \*.o files in Linux. Then we need to link them together to get the output binary.  
In order to compile only without linking, we can use '-c' flag.

`% gcc -c main.c`  
will produce main.o  

`% g++ -c reciprocal.cpp`  

will produce reciprocal.o  

`% g++ main.o reciprocal.o -o reciprocal`  

will produce final output binary file reciprocal.  

If you have any include files that are present in any other directory apart from current directory of source files and system header path then we can use `-I <path>`.  
Suppose we had all the implementation files in `source` folder, and headers in `include` folder, then we can compile using
and assuming we are in source directory  
```
% g++ -I ../include -c reciprocal.cpp
% gcc -I ../include -c main.c
```
If we want to link it to any external library, that can either be  
- `static library`, will have extension `*.a` in Linux, 'a' meaning archive, `*.lib` in windows. These libraries will be loaded in runtime,as and when required by the program. A single shared object will be loaded across multiple binaries that requires this shared object, hence the name 'shared object'.
- `dynamic/shared library`, will have extension `*.so`, 'so' meaning 'shared object' in Linux, `*.dll` in windows, 'dll' meaning 'dynamic link library'. These libraries will be loaded when loading the binary into memory. Each binary will have the a copy of static library linked with it, hence each binary will load a copy of static library into memory with it.


If the libraries are present in the normal path like `/usr/lib`, then it will automatically loaded into memory.  
If the libraries are present in some other folder, we need to tell the compiler/linker the path from which to load the libraries.
This is done using `-L <library_path>` and `-l <library_name>`, where `-L` is used to resolve the path of the library and `-l` is followed by name of the library.  
The library name's are always prefixed with `lib<library_name>`, for instance, if we want to link libstdc++.so, we need to link it with `-lstdc++`, the compiler will add the prefix `lib` and `.so`.  
```
% gcc -o reciprocal reciprocal.o main.o -L /usr/lib/gcc/ -lstdc++
```  
The above gcc command is just for depiction. There is no need to give path to libraries inside /usr/lib as there are already part of `LD_LIBRARY_PATH`. Also the standard library is already linked by default.


## Makefile

In order to make it easier to compile, we are using a makefile.

Makefile is a file used to make targets.
The targets have dependencies and these dependencies should be met to make a target else it fails.


Now our final output reciprocal depends on reciprocal.o and main.o, which in turn depends on compilation of their respective source files and header file

So, we can use this to write makefile. We define target 'reciprocal' with 'main.o' and 'reciprocal.o' as dependencies.  
Once we have the dependencies ready, we need to link it, which we do by 'gcc -o reciprocal reciprocal.o main.o' command. 
Then we define each '.o' files as targets with compilation of their source files using gcc/g++.  
```
reciprocal: reciprocal.o main.o
	gcc ${CFLAGS} -o reciprocal reciprocal.o main.o

reciprocal.o:
	g++ ${CFLAGS} -c reciprocal.c

main.o:
	gcc ${CFLAGS} -c main.c
```

Also if need to rebuild, makefile will automatically detect changes in the file and compile only the once that are dependant on those changes.  
Example, If anything is changed in main.c, then only main.c will be compiled and relinked to form output, but reciprocal.cpp will not be recompiled.
Similarly, if any changes are done in the reciprocal.hpp, both the implementation files are recompiled to form the output.

Makefile also provides a taget called 'clean'. This is used to remove the artifacts produced of previous make command.
So, in this case, we need to remove both .o files and reciprocal binary file to clean.
We can define target clean as follows
```
clean:
	rm -f *.o reciprocal
```

To compile using make, you can use the command,  
`% make`  
If you want to clean  
`% make clean`  

Note that in the above make targets like reciprocal, reciprocal.o, main.o,
we have used ${CFLAGS}, which can be used to pass any compiler flags. ${``<variable>``} is a way to access an environment variable in a makefile. 
If we want to produce debug symbols then we can pass -g option  
`% make CFLAGS="-g"`  
It is better to pass the debug flags in double quotes to avoid any whitespace issues.
If dont want the compiler to do any optimization while compiling then pass 'O0'.  
`-O<level>` is the compiler flag to command the optimization level to compiler. If we provider -O0, it means no optimization.
If you want to produce debug symbols and remove any optimization done by compiler, then   
`% make CFLAGS="-O0 -g"`

Another way is to export CFLAGS and then make  
`% export CFLAGS="-O0 -g"`  
`% make`  

**NOTE:** Whenever you want to compile with different compiler flags, please make sure that you 'clean build', i.e., you clean and remove the artifacts completely
before building, as make file does not understand and does not think that changes in compiler flags requires recompilation.


## Debugging

Now, consider debugging this program, we can use gdb, that is part of developer tools in GNU toolset.

To debug a program, run  
`% gdb <binary_file_to_be_debugged>`  
in this case  
`%gdb reciprocal`  

Then gdb will load the program and all its debug symbols.
Then you need to run the program using 'run' command  
`%(gdb) run`
This will run the program.
Now as soon as you run the reciprocal program in gdb you will face crash or segmentation fault, which is caused by SIGSEGV signal.
To know what is the reason for crash, you can use 'where' command.   
`%(gdb) where`
This will point to the point which is the cause of the crash.
In this case, it is `strtol_internal(ptr = 0x0)`, which has one of its parameter as null parameter.

This is because, we have called `atoi(argv[1])`, in `main()`, which converts the argument passed to main from string to integer.
But while running gdb, we did not pass any argument. We can pass argument when we do give the command 'run' in gdb.  
`%(gdb) run 7`  
here 7 is the argument and reciprocal of 7 is calculated.

We can put a breakpoint in gdb using 'b' followed by the function name where we want to break during execution.
Then we can run the program using gdb using the 'run' command, until the program reaches the breakpoint.  
Like any other debugger, we can step into, step over using 's', 'n' on the gdb prompt.
We can also print the stacktrace, if there is any crash using the backtrace, 'bt' command.  
gdb provides a lot of help options, providing a list of topics, like stack, breakpoints, etc., which you can view by  
```
%(gdb) help <topic>
%(gdb) help stack
```
provides a list of gdb commands to view stacktrace etc.


