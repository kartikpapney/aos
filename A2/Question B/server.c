#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

sem_t x, y;
pthread_t readerthreads[100];
int readercount = 0;
int upload_rate = 10;
int BUFF_SIZE = 1024;
char buff[1024];

void fn(int connFd)
{
	printf("\n============== Sending file to proxy server =======================\n");
	FILE* fd = fopen("server.txt", "r");
	int total_bytes = 0;
	fseek(fd, 0L, SEEK_END);
	total_bytes=ftell(fd);
	printf("Total size of file is %d bytes\n", total_bytes);
	write(connFd, &total_bytes, sizeof(int));
	fseek(fd, 0L, SEEK_SET);
	while(!feof(fd)) {
		int no_of_bytes_read = fread(buff, 1, upload_rate, fd);
		send(connFd,buff,no_of_bytes_read,0);
		printf("Data sent: ");
		for(int i=0; i<no_of_bytes_read; i++) {
			printf("%c", buff[i]);
		}
		printf("\n");
		sleep(1);
	}
	fclose(fd);
	close(connFd);
	printf("\n============== Sent to proxy server =======================\n");
}

// Reader Function
void* serveClient(void* param)
{
    sem_wait(&x);
    readercount++;
    if (readercount == 1) sem_wait(&y);
    sem_post(&x);
    fn(*(int*)param);
    sem_wait(&x);
    readercount--;
    if (readercount == 0) sem_post(&y);
    sem_post(&x);
    pthread_exit(NULL);
}

int main()
{
	socklen_t addr_size;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;
	sem_init(&x, 0, 1);
	sem_init(&y, 0, 1);

	int socketFd = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8989);

	bind(socketFd,
		(struct sockaddr*)&serverAddr,
		sizeof(serverAddr));

	if (listen(socketFd, 50) == 0)
		printf("Server is listening @%d\n", serverAddr.sin_port);
	else
		printf("Error\n");

	int i = 0;

	while (1) {
		addr_size = sizeof(serverStorage);
		int connFd = accept(socketFd,
						(struct sockaddr*)&serverStorage,
						&addr_size);
        if (pthread_create(&readerthreads[i++], NULL,
                        serveClient, &connFd)
            != 0)
            printf("Failed to create thread\n");
	}
	return 0;
}
