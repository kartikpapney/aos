#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>


char buff[1024];
int upload_rate = 10;

int min(int a, int b) {
	if(a < b) return a;
	return b;
}
int total_bytes;
int socketFd;
void fn()
{
	printf("\n==============Downloading File from Proxy =====================\n");
	FILE* fd = fopen("client.txt", "w+");
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
	printf("\n==============Downloaded from Proxy =====================\n");
}

void* clienthread(void* args)
{
	int connection_status;
 	socketFd = socket(AF_INET,SOCK_STREAM, 0);
	struct sockaddr_in server_address;
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY;
	server_address.sin_port = htons(8988);
	
	connection_status = connect(socketFd,
									(struct sockaddr*)&server_address,
									sizeof(server_address));

	if (connection_status < 0) {
		puts("Connection Failed with Proxy\n");
		return 0;
	}
	printf("Connection done with Proxy\n");
	fn();
	close(socketFd);
	pthread_exit(NULL);
	return 0;
}

int main(char* args)
{
	pthread_t tid;
    pthread_create(&tid, NULL,
                clienthread,
                args);
	pthread_join(tid, NULL);
}