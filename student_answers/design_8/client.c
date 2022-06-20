#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include "sock.h"
#include <semaphore.h>
#include <fcntl.h>
#include <time.h>

void report(const char *msg, int terminate) {
	perror(msg);
	if (terminate)
		exit(-1);
}

int main() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("[%d:C][%02d:%02d:%02d][] (Client has been started.)\n", getpid(),
			tm.tm_hour, tm.tm_min, tm.tm_sec);

	printf("[%d:C][%02d:%02d:%02d][] Please enter your name:\n", getpid(),
			tm.tm_hour, tm.tm_min, tm.tm_sec);

	char name[50];
	scanf("%s", name);

	printf("[%d:C][%02d:%02d:%02d][%s] (Client name is set to “%s”)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name, name);

	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		report("socket", 1);

	struct hostent *hptr = gethostbyname("localhost");
	if (!hptr)
		report("gethostbyname", 1);
	if (hptr->h_addrtype != AF_INET)
		report("bad address family", 1);

	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	saddr.sin_port = htons(8000);

	if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0)
		report("connect", 1);

	char buffer[BuffSize + 1];
	memset(buffer, '\0', sizeof(buffer));
	recv(sockfd, buffer, sizeof(buffer), 0);
	int newPort = *(unsigned int*) (buffer);

	close(sockfd);

	//New Socket
	sem_t *semptr = sem_open("socketSemaphore", O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report("sem_open", 1);

	char text[200];
	while (strcmp(text, "exit") != 0) {
		printf(
				"[%d:C][%02d:%02d:%02d][%s] (Trying to send message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		printf(
				"[%d:C][%02d:%02d:%02d][%s] Enter the message you want to send:\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		scanf("%s", text);

		int sockfd1 = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd1 < 0)
			report("socket", 1);

		struct sockaddr_in saddr1;
		memset(&saddr, 0, sizeof(saddr1));
		saddr1.sin_family = AF_INET;
		saddr1.sin_addr.s_addr =
				((struct in_addr*) hptr->h_addr_list[0])->s_addr;
		saddr1.sin_port = htons(newPort);

		if (connect(sockfd1, (struct sockaddr*) &saddr1, sizeof(saddr1)) < 0)
			report("connect", 1);

		sem_wait(semptr);

		if (strcmp(text, "exit") != 0) {
			write(sockfd1, text, strlen(text));

			printf(
					"[%d:C][%02d:%02d:%02d][%s] (Trying to send message:”%s” to the Server.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name, text);

			if (sem_post(semptr) < 0)
				report("sem_post", 1);
		}

		close(sockfd1);
	}
	sem_close(semptr);

	return 0;
}
