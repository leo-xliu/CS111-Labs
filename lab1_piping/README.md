# Pipe Up

A low-level C implementation of piping in the linux command line that allows the user to simulate the same functionality as '|' with the limitation that no commands take arguments or flags.

## Building

To build the project, type 'make' in the command line and press enter. This will create a pipe object file as well as an executable. This is all that is needed to begin running the program. 

## Running

To run the program, we will first type './pipe' which will execute our program. Then on the same command, we must add the arguments which will be the commands to be piped. For example, './pipe ls cat wc' will take the 3 arguments 'ls', 'cat', and 'wc' and process them as commands and redirect their input/output so they are being piped. From this, we expect that stdin goes into 'ls' whose results go into 'cat' and lastly goes into 'wc'. Then 'wc' will output to stdout where the it will give the count information based on the results from the pipe. 

This pipe command can be used with any finite number of arguments and will pipe everything that is sent as arguments. Giving arguments that are not actual commands will result in error and so will providing no arguments.

## Cleaning up

To clean up, all we need to do is type 'make clean' in the command line and press enter. This will take care of everything for you. 
