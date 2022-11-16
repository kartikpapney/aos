#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){

	/*
		In this code the child process is terminated before the parent process and the parent waits for the child process.
		status comes out to be 0
	*/
	pid_t pid=fork();
	int status;
	if(pid==0){
		// child process
		printf("Child process Id :%d\nChild parent process Id : %d\n",getpid(), getppid());
		exit(0);
	} else if(pid<0){
		exit(1);
	} else	{ 
		// parent process
		pid = waitpid(pid, &status, 0);
		printf("Parent waits for the child process then Status = %d\n", status);
		system("ps ajx | grep fork");
	}
	exit(0);
}
