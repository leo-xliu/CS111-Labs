# A Kernel Seedling

A kernel seedling is code that can be loaded into the kernel to provide additionally functionality to the linux kernel.

## Building

To build my kernel module, simply type 'make' in the module directory and press enter. This will compile my proc_count.o file and create a proc_count.ko file. It will also build everything necessary for us to run the kernel module.

## Running

To run the kernel module with the newly created proc_count.ko, type 'sudo insmod pro_count.ko' and press enter. This will run the initialization function within the source code. Within the initialization function, it will create a file called 'count' in the /proc directory. So when we run 'cat /proc/count' it will run my show function (which was in the argument for creating 'count') which iterates through every processes and keeps count of total amount. Then when it has iterated through all processes, it will print the result as output. 

## Cleaning Up

To clean up, type 'sudo rmmod proc_count' and press enter which will run the cleanup function associated with my module. Inside the cleanup function is the function 'proc_remove' which will remove /proc/count. If you want to clean up the compiled files such as 'proc_count.ko', type 'make clean' and press enter. 

## Testing

Module was tested on kernel release v5.14.8.

