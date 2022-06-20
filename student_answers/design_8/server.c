#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include "sock.h"
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>
#include <pthread.h>

static int port = 8000;

void report(const char *msg, int terminate) {
	perror(msg);
	if (terminate)
		exit(-1);
}

void* newThread(void *vargp) {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		report("socket", 1);

	struct sockaddr_in saddr1;
	memset(&saddr1, 0, sizeof(saddr1));
	saddr1.sin_family = AF_INET;
	saddr1.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr1.sin_port = htons(port);

	if (bind(fd, (struct sockaddr*) &saddr1, sizeof(saddr1)) < 0)
		report("bind", 1);

	if (listen(fd, MaxConnects) < 0)
		report("listen", 1);

	while (1 == 1) {
		struct sockaddr_in caddr;
		int len = sizeof(caddr);

		int client_fd = accept(fd, (struct sockaddr*) &caddr, &len);
		if (client_fd < 0) {
			report("accept", 0);
			continue;
		}

		printf("[%d:P][%02d:%02d:%02d][Server] (Server received a message.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		char buffer[BuffSize + 1];
		memset(buffer, '\0', sizeof(buffer));
		int count = read(client_fd, buffer, sizeof(buffer));
		if (count > 0 && strcmp(buffer, "exit") != 0) {
			puts(buffer);
		}

		close(client_fd);
	}
	return NULL;
}

int main() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been started.)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		report("socket", 1);

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(8000);

	if (bind(fd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0)
		report("bind", 1);

	if (listen(fd, MaxConnects) < 0)
		report("listen", 1);

	sem_t *semptr = sem_open("socketSemaphore", O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report("sem_open", 1);

	sem_init(semptr, 0, 1);

	while (1 == 1) {
		struct sockaddr_in caddr;
		int len = sizeof(caddr);

		int client_fd = accept(fd, (struct sockaddr*) &caddr, &len);
		if (client_fd < 0) {
			report("accept", 0);
		}

		port++;
		unsigned int newPort = (unsigned int) port;
		send(client_fd, (char*) &newPort, sizeof(unsigned int), 0);

		pthread_t thread_id;
		pthread_create(&thread_id, NULL, newThread, NULL);

		close(client_fd);
	}

	return 0;
}
