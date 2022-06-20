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
#include <stdbool.h>
#include <time.h>

void report(const char *msg, int terminate) {
	perror(msg);
	if (terminate)
		exit(-1); /* failure */
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

	sem_t *semptr = sem_open("socketSemaphore", O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report("sem_open", 1);

	bool again = false;

	/* Write some stuff and read the echoes. */
	char text[200];
	while (strcmp(text, "exit") != 0 || again) {
		if (!again) {
			printf(
					"[%d:C][%02d:%02d:%02d][%s] (Trying to send message in while loop.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

			printf(
					"[%d:C][%02d:%02d:%02d][%s] Enter the message you want to send:\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);
			scanf("%s", text);
		}

		/* fd for the socket */
		int sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if (sockfd < 0)
			report("socket", 1);

		/* get the address of the host */
		struct hostent *hptr = gethostbyname("localhost");
		if (!hptr)
			report("gethostbyname", 1);
		if (hptr->h_addrtype != AF_INET)
			report("bad address family", 1);

		/* connect to the server: configure server's address 1st */
		struct sockaddr_in saddr;
		memset(&saddr, 0, sizeof(saddr));
		saddr.sin_family = AF_INET;
		saddr.sin_addr.s_addr =
				((struct in_addr*) hptr->h_addr_list[0])->s_addr;
		saddr.sin_port = htons(PortNumber); /* port number in big-endian */

		if (connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0)
			report("connect", 1);

		sem_wait(semptr);

		if (again) {
			unsigned int pid = getpid();
			write(sockfd, (char*) &pid, sizeof(unsigned int));
			again = false;
		} else {
			printf(
					"[%d:C][%02d:%02d:%02d][%s] (Trying to send message:”%s” to the Server.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name, text);

			write(sockfd, text, strlen(text));
		}

		if (sem_post(semptr) < 0)
			report("sem_post", 1);

		if (strcmp(text, "killme") == 0) {
			again = true;
			text[0] = "x";
		} else {
			again = false;
		}

		close(sockfd);
	}
	sem_close(semptr);

	printf("[%d:C][%02d:%02d:%02d][%s] (Client has been exiting)\n", getpid(),
			tm.tm_hour, tm.tm_min, tm.tm_sec, name);

	return 0;
}
