# Unix Help Topics

Unix provides documentation which helps in using the system features/calls.
They are in the form of
* man pages - manual pages for each of the unix system feature/call.
* info pages - information pages topicwise, which are set of pages in markup language to navigate between pages.

### man pages

The manual pages are divided into various sections and numbered accordingly
(1) User Commands  
(2) System Calls  
(3) Standard Library functions
(8) System/Administrative Commands

Depending on the utility or system feature used, it may have various manual pages in the fields it is used.
Example:  
	`sleep command`
sleep is an utility/system function that can be used on bash or linux system.
It can also be used in user programs to make the process/thread sleep for specified amount of time.

Linux provides documentation for system sleep command as  
`sleep (1)`  
and as a standard library function as  
`sleep (3)`  

You can see the manual page for sleep just by
`% man sleep`  
which be default opens user command manual.  
If we want to read the documentation for standard library function use  
`% man 3 sleep`  


### whatis

In order to know the available documentation for a linux command, use  
`whatis <what>`  
where `<what>` can be replaced for any command, eg: sleep.
So `whatis sleep` will provide the following output  
- sleep (1)	-	delay for a specified amount of time
- sleep (3)	-	sleep for specified number of seconds

`whatis` can be used with command to find the avaiable documentation for the same.


### info

It provides complete documentation of all the availble commands in a well categorized format.
This documentation is in hypertext format, which can be easily navigated.

`info` provides `root` level documentation.
`info <command>` provides documentation of that specified command, but we can still navigate to other topics as it is rendred as navigatable pages.



