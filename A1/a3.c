#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]){

	/*
		Here Parent terminates before child.

        Result comes out to be parent process id = 1576
	*/
	pid_t pid=fork();
	int status;
	if(pid==0){
		sleep(2);
		// child process
        // while(1) {

		// }
        pid = getppid();
        printf("Parent process id: %d\n", pid);
		system("ps ajx | grep fork");
	} else if(pid<0) {
		exit(1);
	} else	{ 
		// parent process
		exit(0);
	}
}
