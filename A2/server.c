// C program for the Server Side

// inet_addr
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
pthread_t tid;
pthread_t readerthreads[100];
int readercount = 0;

void* reader(void* param)
{
	sem_wait(&x);
	readercount++;

	if (readercount == 1)
		sem_wait(&y);

	sem_post(&x);

	printf("Hello %d\n",
		readercount);

	sleep(5);
	sem_wait(&x);
	readercount--;

	if (readercount == 0) {
		sem_post(&y);
	}
	sem_post(&x);

	printf("\nBye %d\n",
		readercount + 1);
	pthread_exit(NULL);
}

int main()
{
	int serverSocket, newSocket;
	struct sockaddr_in serverAddr;
	struct sockaddr_storage serverStorage;

	socklen_t addr_size;
	sem_init(&x, 0, 1);
	sem_init(&y, 0, 1);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(8989);

	bind(serverSocket,
		(struct sockaddr*)&serverAddr,
		sizeof(serverAddr));

	if (listen(serverSocket, 50) == 0)
		printf("Listening\n");
	else
		printf("Error\n");

	pthread_t tid[60];

	int i = 0;

	while (1) {
		addr_size = sizeof(serverStorage);

		newSocket = accept(serverSocket,
						(struct sockaddr*)&serverStorage,
						&addr_size);
		int choice = 0;
		recv(newSocket,
			&choice, sizeof(choice), 0);

        if (pthread_create(&readerthreads[i++], NULL,
                        reader, &newSocket)
            != 0)
            printf("Failed to create thread\n");
	}

	return 0;
}
