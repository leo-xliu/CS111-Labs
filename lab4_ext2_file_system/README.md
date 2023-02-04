# Hey! I'm Filing Here

Creation of a mountable 1 MiB EXT2 File System with 2 directories, 1 regular file, and 1 symbolic link.

## Building

To build the executable, run 'make' in the lab4 directory. This will create the executable needed to run the file system. 

## Running

Now that we have built the executable, we must run it to create an image for the file system using './ext2-create'. Then we can create a directory which our file system will mount to: run 'mkdir mnt' to create a directory called 'mnt'. Since we are mounting an image, we will run 'sudo mount -o loop cs111-base.img mnt' which will actually mount our file system. Now that the file system is mounted, we can enter the 'mnt' directory using 'cd mnt'. From here, 'mnt' exists as our root directory for the EXT2 file system. We can run 'ls -ain' which should output the following if correctly mounted: 

total 7
  2 drwxr-xr-x 3    0    0 1024 Nov 28 15:10 .
313 drwxr-xr-x 4 1000 1000 4096 Nov 28 15:10 ..
 13 lrw-r--r-- 1 1000    0   11 Nov 28 15:10 hello -> hello-world
 12 -rw-r--r-- 1 1000 1000   12 Nov 28 15:10 hello-world
 11 drwxr-xr-x 2    0    0 1024 Nov 28 15:10 lost+found

## Cleaning up

First, we must unmount the filesystem. Return back to the lab4 directory and run 'sudo umount mnt' which will unmount the file system. Then to remove the mnt directory run 'rmdir mnt'. Lastly, run 'make clean' to clean up all files created from building. 
