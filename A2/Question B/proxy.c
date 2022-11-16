#include <arpa/inet.h>

// For threading, link with lpthread
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdbool.h>    // bool type

sem_t x, y;
pthread_t readerthreads[100];
int readercount = 0;
int upload_rate = 10;
int BUFF_SIZE = 1024;
char buff[1024];
int socketFd;
int total_bytes = 0;

bool file_exists (char *filename) {
	FILE* fd = fopen(filename, "r");
	fseek(fd, 0L, SEEK_END);
	int total_bytes = ftell(fd);
	printf("File size = %d\n", total_bytes);
	return (total_bytes != 0);
}

int min(int a, int b) {
	if(a<b) return a;
	return b;
}
void fn(int connFd)
{
	printf("\n============== Sending file to client =======================\n");
	FILE* fd = fopen("proxy.txt", "r");
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
	printf("\n============== Sent to client =======================\n");
}

void getFileFromServer() {
	printf("\n==============Downloading File from Server =====================\n");
	FILE* fd = fopen("proxy.txt", "w+");
	if(read(socketFd, &total_bytes, sizeof(int)) == -1) {
		printf("Reading total bytes failed");
		return;
	}
	printf("Total Data = %d\n", total_bytes);
	int no_of_bytes_read = 0;
	while(no_of_bytes_read != total_bytes) {
		int to_read = min(upload_rate, total_bytes-no_of_bytes_read);
		read(socketFd, buff, to_read*sizeof(char));
		printf("Data Recieved: ");
		for(int i=0; i<to_read; i++) {
			printf("%c", buff[i]);
			fprintf(fd, "%c", buff[i]);
		}
		printf("\n");
		no_of_bytes_read+=to_read;
	}
	fclose(fd);
	printf("\n==============Downloaded from Server =====================\n");
}

void serverFn() {
	int connection_status;
 	socketFd = socket(AF_INET,SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8989);
	
	connection_status = connect(socketFd,
									(struct sockaddr*)&server_address,
									sizeof(server_address));

	if (connection_status < 0) {
		puts("Connection Failed with Server\n");
		return;
	}
	printf("Connection done with Server\n");
	getFileFromServer();
	close(socketFd);
}

// Reader Function
void* serveClient(void* param)
{
    sem_wait(&x);
    readercount++;
    if (readercount == 1) sem_wait(&y);
    sem_post(&x);
	if (!file_exists("proxy.txt")) {
		serverFn();
	}
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
	serverAddr.sin_port = htons(8988);

	bind(socketFd,
		(struct sockaddr*)&serverAddr,
		sizeof(serverAddr));

	if (listen(socketFd, 50) == 0)
		printf("Proxy is listening @%d\n", serverAddr.sin_port);
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
