# You Spin Me Round Robin

Implement a simple round robin scheduling program that takes advantage of TAILQ, a doubly-linked list library, to schedule all processes and output the average response time and wait time. 

## Building

To build the program, type 'make' and press enter in the directory of the program which will do all the compilation for you.

## Running

To run, we must provide the executable with two arguments, the file containing all the processes we want to schedule and an integer for quantum length. 
For example, if I have the processes in 'processes.txt', and want quantum length of 3, I type './rr processes.txt 3' in the directory of the program.
The output will be: 
Average waiting time: 7.00
Average response time: 2.75

## Cleaning up

To clean up all the files, we just need to type 'make clean' and press enter in the directory of the program. 
