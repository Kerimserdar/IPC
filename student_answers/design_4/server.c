#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <time.h>

void report_and_exit(const char *msg) {
	perror(msg);
	exit(-1);
}

int main() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been started.)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	const char *file = "./fifoChannel";
	int fd = open(file, O_RDONLY);
	if (fd < 0)
		return -1;

	sem_t *semptr = sem_open("Design_4_Semaphore",
	O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report_and_exit("sem_open");

	while (1 == 1) {
		printf(
				"[%d:P][%02d:%02d:%02d][Server] (Trying to read message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		printf("[%d:P][%02d:%02d:%02d][Server] (Server is going to sleep.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		if (!sem_wait(semptr)) {
			printf("[%d:P][%02d:%02d:%02d][Server] (Server woke up.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

			char text[200];
			int count = read(fd, text, sizeof(text));
			if (count > 0) {
				printf(
						"[%d:P][%02d:%02d:%02d][Server] (Server received message as:”%s”.)\n",
						getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, text);
			}
		}
	}
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been exiting)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	close(fd);
	sem_close(semptr);
	unlink(file);

	return 0;
}
