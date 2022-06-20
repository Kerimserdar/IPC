#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#define FileName "shared_file.txt"

void report_and_exit(const char *msg) {
	perror(msg);
	exit(-1); /* EXIT_FAILURE */
}

void report_and_sleep(const char *msg) {
	perror(msg);
	sleep(1);
}

int main() {
	bool x = true;

	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been started.)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	char old[200];
	while (1 == 1) {
		if(x)
		printf(
				"[%d:P][%02d:%02d:%02d][Server] (Trying to read message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		if(x)
		printf("[%d:P][%02d:%02d:%02d][Server] (Server is going to sleep.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

		sleep(1);

		if(x)
		printf("[%d:P][%02d:%02d:%02d][Server] (Server woke up.)\n", getpid(),
				tm.tm_hour, tm.tm_min, tm.tm_sec);

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();

		int fd;
		if ((fd = open(FileName, O_RDONLY)) < 0)
			report_and_exit("open to read failed...");

		fcntl(fd, F_GETLK, &lock);
		if (lock.l_type != F_UNLCK)
			report_and_sleep("file is still write locked...");

		lock.l_type = F_RDLCK;
		if (fcntl(fd, F_SETLK, &lock) < 0)
			report_and_exit("can't get a read-only lock...");

		char text[200];
		x = false;
		int count = read(fd, text, sizeof(text));
		if (count > 0 && strcmp(text, old) != 0) {
			printf(
					"[%d:P][%02d:%02d:%02d][Server] (Server received message as:”%s”.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, text);
			strcpy(old, text);
			x = true;
		}

		lock.l_type = F_UNLCK;
		if (fcntl(fd, F_SETLK, &lock) < 0)
			report_and_exit("explicit unlocking failed...");

		close(fd);
	}
	printf("[%d:P][%02d:%02d:%02d][Server] (Server has been exiting)\n",
			getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec);

	return 0;
}
