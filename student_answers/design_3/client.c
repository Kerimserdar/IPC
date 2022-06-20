#include <sys/wait.h> /* wait */
#include <stdio.h>
#include <stdlib.h>   /* exit functions */
#include <unistd.h>   /* read, write, pipe, _exit */
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>

void report_and_exit(const char *msg) {
	perror(msg);
	exit(-1); /** failure **/
}

int main(void) {
	int pipeFDs[2]; /* two file descriptors */
	char buf; /* 1-byte buffer */

	if (pipe(pipeFDs) < 0)
		report_and_exit("pipeFD");

	// create, initialize semaphores
	sem_t *sem1 = sem_open("/semaphore1", O_CREAT, 0644, 0);
	sem_t *sem2 = sem_open("/semaphore2", O_CREAT, 0644, 0);

	sem_init(sem1, 0, 0);
	sem_init(sem2, 0, 1);

	int value;
	sem_getvalue(sem1, &value);
	printf("%d\n", value);

	if (fork()) {
		if (!sem_wait(sem1)) { /* wait until semaphore != 0 */
			printf("Parent \n");
			close(pipeFDs[1]);

			while (read(pipeFDs[0], &buf, 1) > 0) /* read until end of byte stream */
				write(STDOUT_FILENO, &buf, sizeof(buf)); /* echo to the standard output */

			close(pipeFDs[0]);

			sem_post(sem2); // Release the semaphore lock
		}
		wait(NULL);
		exit(0);

	} else {
		if (!sem_wait(sem2)) { /* wait until semaphore != 0 */
			printf("Child \n");
			char text[200];
			scanf("%s", text);

			close(pipeFDs[0]);
			write(pipeFDs[1], text, strlen(text)); /* write the bytes to the pipe */
			close(pipeFDs[1]);

			sem_post(sem1); // Release the semaphore lock
			_exit(0);
		}
	}

	// Close the Semaphores
	sem_close(sem1);
	sem_unlink("/semaphore1");
	sem_close(sem2);
	sem_unlink("/semaphore2");
	return 0;
}
