#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include<time.h>

char **parse(char *inp) {
    char **command = malloc(20 * sizeof(char *));
    int index = 0;

    inp = strtok(inp, " ");
    while (index < 19 && inp != NULL) {
        command[index] = inp;
        index++;
        inp = strtok(NULL, " ");
    }
    command[index] = NULL;
    return command;
}

int main() {
    char** command;
    char* input;
    int stat_loc;
    while(1) {
        input = (char*) malloc(20 * sizeof(char));
        printf(">> ");
        scanf("%s", input);
        command = parse(input);
        printf("%s\n", command[0]);
        if(command[0]) {
            int child_pid = fork();
            if (child_pid == 0) {
                execvp(command[0], command);
                printf("Please enter valid command!!");
            } else {
                waitpid(child_pid, &stat_loc, WUNTRACED);
            }
        }
        free(input);
        free(command);
    }
    return 0;
}