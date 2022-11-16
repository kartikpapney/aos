#include<stdio.h>
#include <pthread.h>

void *thread(void *vargp)
{
    printf("Hello World!\n"); 
    pthread_exit((void *)NULL);
}


int main() 
{
    pthread_t tid;
    if(pthread_create(&tid, NULL, thread, NULL) != 0) {
        printf("Error while creating thread");
        return 0;
    }
    pthread_join(tid, NULL);
    pthread_exit((void *)NULL);
}

