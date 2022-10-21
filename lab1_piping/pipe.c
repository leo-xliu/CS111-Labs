#include<stdlib.h>
#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/wait.h> 

int main(int argc, char *argv[])
{
	if (argc == 1 ){
		fprintf(stderr, "No command line arguments\n");
		return EINVAL;
	}

	int pid; 
	int prevfd = STDIN_FILENO;
	int i = 1;
	for (; i < argc-1; i++){
		int fd[2];
		if(pipe(fd) == -1){
			fprintf(stderr, "Pipe failed");
			exit(1);
		}
		pid = fork();
		if (pid == 0){
			if (i != 1){
				dup2(prevfd, STDIN_FILENO);
				close(prevfd);
			}
			dup2(fd[1], STDOUT_FILENO);
			close(fd[0]);
			close(fd[1]);
			if (execlp(argv[i], argv[i], NULL) == -1){
				return errno;
			}
		}
		else if (pid > 0){
			if (i != 1){
				dup2(prevfd, STDIN_FILENO);
				close(prevfd);
			}
			close(fd[1]);
			int status;
			wait(&status);
			if (!WIFEXITED(status)){
				fprintf(stderr, "Child process did not terminate correctly");
				return ECHILD;
			}
			int exit_status = WEXITSTATUS(status);
			if (exit_status){
				fprintf(stderr, "Invalid command(s)\n");
				return ECHILD;
			}
			prevfd = fd[0];
		}
		else {
			fprintf(stderr, "Child process not created");
			return ECHILD;
		}
	}

	//for special case argc = 2 and last command
	pid = fork();
	if (pid == 0){
		if (argc != 2){
			dup2(prevfd, STDIN_FILENO);
			close(prevfd);
		}
		if (execlp(argv[i], argv[i], NULL) == -1){
			return errno;
		}
	}
	else if (pid > 0){
		if (argc != 2){
			close(prevfd);
		}
		int status;
		wait(&status);
		if (!WIFEXITED(status)){
			fprintf(stderr, "Child process did not terminate correctly");
			return ECHILD;
		}
		int exit_status = WEXITSTATUS(status);
		if (exit_status){
			fprintf(stderr, "Invalid command(s)\n");
			return ECHILD;
		}
	}
	else{
		fprintf(stderr, "Child process not created");
		return ECHILD;
	}

	return 0;
}
