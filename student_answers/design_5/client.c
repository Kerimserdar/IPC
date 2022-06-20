#include <stdio.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include "queue.h"
#include <fcntl.h>
#include <time.h>

void report_and_exit(const char *msg) {
	perror(msg);
	exit(-1); /* EXIT_FAILURE */
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

	key_t key = ftok("queue.h", 1);
	if (key < 0)
		report_and_exit("couldn't get key...");

	int qid = msgget(key, 0666 | IPC_CREAT);
	if (qid < 0)
		report_and_exit("couldn't get queue id...");

	sem_t *semptr = sem_open("mqueueSemaphore", O_CREAT, 0644, 0);
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

		if(strcmp(text, "exit") != 0) {
			sem_wait(semptr);
			queuedMessage msg;
			msg.type = 1;
			strcpy(msg.payload, text);

			printf(
					"[%d:C][%02d:%02d:%02d][%s] (Trying to send message:”%s” to the Server.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name, text);

			msgsnd(qid, &msg, sizeof(msg), 0);

			if (sem_post(semptr) < 0)
				report_and_exit("sem_post");
		}

	}

	printf("[%d:C][%02d:%02d:%02d][%s] (Client has been exiting)\n", getpid(),
			tm.tm_hour, tm.tm_min, tm.tm_sec, name);

	sem_close(semptr);

	return 0;
}

