#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <semaphore.h>
#include <string.h>
#include <time.h>

void report_and_exit(const char *msg) {
	perror(msg);
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

	const char *pipeName = "./fifoChannel";
	mkfifo(pipeName, 0666);
	int fd = open(pipeName, O_CREAT | O_WRONLY);
	if (fd < 0)
		return -1;

	sem_t *semptr = sem_open("Design_4_Semaphore",
	O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report_and_exit("sem_open");

	char text[200];
	while (strcmp(text, "exit") != 0) {
		printf(
				"[%d:C][%02d:%02d:%02d][%s] (Trying to send message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		printf(
				"[%d:C][%02d:%02d:%02d][%s] Enter the message you want to send:\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		scanf("%s", text);

		if (strcmp(text, "exit") != 0)
			write(fd, text, sizeof(text));

		if (sem_post(semptr) < 0) {
			printf(
					"[%d:C][%02d:%02d:%02d][%s] (Trying to send message:”%s” to the Server.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name, text);

			report_and_exit("sem_post");
		}

	}

	printf("[%d:C][%02d:%02d:%02d][%s] (Client has been exiting)\n", getpid(),
			tm.tm_hour, tm.tm_min, tm.tm_sec, name);

	sem_wait(semptr);

	sem_close(semptr);
	close(fd);
	unlink(pipeName);

	return 0;
}
