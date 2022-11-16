/**
 * In this program, you&#39;ll write a program to solve the m-producer n-consumer problem, m, n&gt;= 1.
    You have a shared circular buffer that can hold 20 integers. Each of the producer processes
    stores the numbers 1 to 50 in the buffer one by one (in a for loop with 50 iterations) and then
    exits. Each of the consumer processes reads the numbers from the buffer and adds them to a
    shared variable SUM (initialized to 0). Any consumer process can read any of the numbers in the
    buffer. The only constrain is that every number written by some producer should be read exactly
    once by exactly one of the consumers.
    Of course, a producer should not write when the buffer is full and a consumer should not read
    when the buffer is empty.
    Write a program that first creates the shared circular buffer and the shared variable SUM using
    the shm*() calls in Linux. You can create any other shared variable that you think you may need.
    The program then reads in the value of m and n from the user, and forks m producers and n
    consumers. The producer and consumer codes can be written as functions that are called by the
    child processes. After all the producers and consumers have finished (the consumers exit after all
    the data produced by all the producers have been read. How does a consumer know this?), the
    parent process prints the value of SUM. Note that the value of SUM should be m*25*51 if your
    program is correct.

    Test your program with at least
    (a) m=1, n=1,
    (b) m=1, n=2,
    (c) m=2, n=1, and
    (d) m=2, n=2.
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
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#define SHM_SIZE 1024
int* sum;

int* in = 0, *out = 0, size = 20;
int* buffer;
sem_t* empty, *full, *m;
int shmid;
key_t key;
struct timespec t;

void* producer(void* param)
{
    printf("producer\n");
    for(int i=1; i<=50; ) {
        sem_wait(empty);
        sem_wait(m);
        printf("producer enters\n");
        buffer[*in] = i;
        *in = (*in+1)%size;
        i++;
        sem_post(m);
        sem_post(full);
        printf("producer exits\n");
    }
    pthread_exit(NULL);
}

void* consumer(void* args)
{
    int mm = *(int*)args;
    while(*sum != 25*51*mm) {
        if(sem_timedwait(full, &t) < 0) continue;;
        sem_wait(m);
        printf("consumer enters\n");
        *sum+=buffer[*out];
        *out = (*out+1)%size;
        sem_post(m);
        sem_post(empty);
        printf("consumer exits\n");
    }
    pthread_exit(NULL);
}

int main() {
    t.tv_sec = 1;
    empty = (sem_t*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    full = (sem_t*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    m = (sem_t*) mmap(NULL, sizeof (int) , PROT_READ | PROT_WRITE,
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    buffer = mmap(NULL, 20*sizeof(int),
                  PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
                  -1, 0);
    in = mmap(NULL, sizeof(int),
                  PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
                  -1, 0);
    out = mmap(NULL, sizeof(int),
                  PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
                  -1, 0);
    sum = mmap(NULL, sizeof(int),
                  PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED,
                  -1, 0);
    sem_init(empty, 1, size);
    sem_init(full, 1, 0);
    sem_init(m, 1, 1);
    key = ftok("file.txt", 'R');
    int fd, stat_loc = 0;
    pid_t child_a, child_b;
    pid_t id = fork();
    if(id == 0) {
        pthread_t *t1, *t2;
        int n = 1, m = 3;
        // scanf("%d %d", &n, &m);
        t1 = (pthread_t*)malloc(sizeof(int)*n);
        t2 = (pthread_t*)malloc(sizeof(int)*m);
        
        for(int t=0; t<n; t++) {
            if(pthread_create(&t1[t], NULL, producer, NULL) != 0) {
                printf("Error while creating thread");
                return 0;
            }
        }
        for(int t=0; t<m; t++) {
            if(pthread_create(&t2[t], NULL, consumer, &n) != 0) {
                printf("Error while creating thread");
                return 0;
            } 
        }

        for(int t=0; t<n; t++) {
            pthread_join(t1[t], NULL);
        }
        for(int t=0; t<m; t++) {
            pthread_join(t2[t], NULL);
        }
        printf("Done\n");
    } else if(id < 0) {
        
    } else {
        // parent
        waitpid(id, &stat_loc, WUNTRACED);
        printf("Final sum sored in shared variable = %d!!\n", *sum);
        shmctl(shmid, IPC_RMID, NULL);
    }

}