#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){

	/*
		In this code the child process is terminated before the parent process and the parent does not wait for the child process.
	*/
	pid_t pid=fork();
	if(pid==0){
		// child process
		printf("Child process Id :%d\nChild parent process Id : %d\n",getpid(), getppid());
		exit(0);
	} else if(pid<0){
		exit(1);
	} else	{ 
		// parent process
		sleep(2);
		system("ps ajx | grep fork");
	}
	exit(0);
}
