#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/msg.h>
#include <stdlib.h>
#include "queue.h"
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>

void report_and_exit(const char *msg) {
	perror(msg);
	exit(-1); /* EXIT_FAILURE */
}

int main() {
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been started.)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	key_t key = ftok("queue.h", 1); /* key to identify the queue */
	if (key < 0)
		report_and_exit("key not gotten...");

	int qid = msgget(key, 0666 | IPC_CREAT); /* access if created already */
	if (qid < 0)
		report_and_exit("no access to queue...");

	sem_t *semptr = sem_open("mqueueSemaphore", O_CREAT, 0644, 0);
	if (semptr == (void*) -1)
		report_and_exit("sem_open");

	sem_init(semptr, 0, 1);

	while (1 == 1) {
		printf(
				"[%d:P][%02d:%02d:%02d][Server] (Trying to read message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		queuedMessage msg;
		if (msgrcv(qid, &msg, sizeof(msg), 1, MSG_NOERROR) < 0)
			report_and_exit("msgrcv trouble...");

		printf(
				"[%d:P][%02d:%02d:%02d][Server] (Server received a message.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		if (strcmp(msg.payload, "exit") != 0)
			printf("%s\n", msg.payload);
	}

	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been exiting)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	/** remove the queue **/
	if (msgctl(qid, IPC_RMID, NULL) < 0)
		report_and_exit("trouble removing queue...");

	return 0;
}
