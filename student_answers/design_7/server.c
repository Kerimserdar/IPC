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
#include <signal.h>
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
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been started.)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
		report("socket", 1);

	/* bind the server's local address in memory */
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(PortNumber);

	if (bind(fd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0)
		report("bind", 1);

	/* listen to the socket */
	if (listen(fd, MaxConnects) < 0) /* listen for clients, up to MaxConnects */
		report("listen", 1);

	sem_t *semptr = sem_open("socketSemaphore", O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report("sem_open", 1);

	sem_init(semptr, 0, 1);

	bool again = false;

	/* a server traditionally listens indefinitely */
	while (1 == 1 || again) {
		printf(
				"[%d:P][%02d:%02d:%02d][Server] (Trying to read message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		struct sockaddr_in caddr; /* client address */
		int len = sizeof(caddr); /* address length could change */

		printf("[%d:P][%02d:%02d:%02d][Server] (Server is going to sleep.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		int client_fd = accept(fd, (struct sockaddr*) &caddr, &len); /* accept blocks */
		if (client_fd < 0) {
			report("accept", 0); /* don't terminated, though there's a problem */
			continue;
		}

		printf("[%d:P][%02d:%02d:%02d][Server] (Server woke up.)\n", getpid(),
				tm.tm_hour, tm.tm_min, tm.tm_sec);

		printf("[%d:P][%02d:%02d:%02d][Server] (Server received a message.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		/* read from client */
		char buffer[BuffSize + 1];
		memset(buffer, '\0', sizeof(buffer));
		int count = read(client_fd, buffer, sizeof(buffer));
		if (count > 0 && strcmp(buffer, "killme") == 0) {
			again = true;
		} else if (count > 0 && strcmp(buffer, "exit") != 0) {
			if (again) {
				int val = *(unsigned int*) (buffer);
				kill(val, SIGKILL);
				again = false;
			} else {
				puts(buffer);
			}

		}

		close(client_fd);
	}

	return 0;
}

