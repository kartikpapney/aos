/**
 * 
 * Consider a system with one parent process and two child processes A and B. There is a shared
    signed integer X initialized to 0. Process A increments X by one 10 times in a for loop. Process
    B decrements X by one 10 times in a for loop. After both A and B finish, the parent process
    prints out the final value of X.
    Declare a shared memory variable to hold X (see the calls shmget(), shmat(), shmdt(), and
    shmctl() in Linux). Write the programs for processes A and B. Do not put any synchronization
    code in the code for A and B . You should write the code in such a way so that you can simulate
    race condition in your program by slowing down A or B appropriately by using sleep() calls at
    appropriate points. Note that if there is no race condition, the value of X finally should be 0.
    Simulating race conditions means that if you run the program a few times, sometimes the final
    value of X printed by your program should be non-zero.

    Add synchronization code based on semaphores toprocess A and B above so that there is no
    possibility of race conditions. Use the calls semget(), semop(), semctl() in Linux to create and
    manage sempahores.
*/




#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <sys/mman.h>

#define SHM_SIZE 1024
int start = 0;
sem_t* y;
int *data;

int main() {
    y = (sem_t*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    sem_init(y, 1, 1);
    int shmid, mode;
    int status;
    key_t key = ftok("file.txt", 'R');
    shmid = shmget(key, SHM_SIZE, 0666 | IPC_CREAT);
    // sprintf(data, "%d", start);
    if(shmid == -1) {
        perror("shmid");
        exit(1);
    }
    int fd;
    pid_t child_a, child_b;
    child_a = fork();

    if (child_a == 0) {
        /* Child A code */
        for(int i=1; i<=10000; i++) {
            sem_wait(y);
            data = shmat(shmid, (void *)0, 0);
            *data = *data - 1;
            sem_post(y);
        }
    } else if(child_a > 0) {
        child_b = fork();
        if(child_b == 0) {
            for(int i=1; i<=10000; i++) {
                sem_wait(y);
                data = (int*) shmat(shmid, (void *)0, 0);
                *data = *data + 1;
                sem_post(y);
            }
        } else if(child_b > 0) {
            waitpid(child_a, &status, WUNTRACED);
            waitpid(child_b, &status, WUNTRACED);

            data = shmat(shmid, (void *)0, 0);
            printf("Value (Synchronized): %d\n", *data);

            if(shmdt(data) == -1) {
                perror("shmdt");
                exit(1);
            }

            shmctl(shmid, IPC_RMID, NULL);
        }
    }
}