#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <time.h>

#define FileName "shared_file.txt"

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

	char text[256];
	while (1 == 1) {
		printf(
				"[%d:C][%02d:%02d:%02d][%s] (Trying to send message in while loop.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		printf(
				"[%d:C][%02d:%02d:%02d][%s] Enter the message you want to send:\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		printf("[%d:C][%02d:%02d:%02d][%s] (Client is going to sleep.)\n",
				getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);
		scanf("%s", text);

		printf("[%d:C][%02d:%02d:%02d][%s] (Client woke up.)\n", getpid(),
				tm.tm_hour, tm.tm_min, tm.tm_sec, name);

		if (strcmp(text, "exit") == 0) {
			printf("[%d:C][%02d:%02d:%02d][%s] (Client has been exiting)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name);

			break;
		}

		struct flock lock;
		lock.l_type = F_WRLCK;
		lock.l_whence = SEEK_SET;
		lock.l_start = 0;
		lock.l_len = 0;
		lock.l_pid = getpid();

		int fd;
		if ((fd = open(FileName, O_RDWR | O_CREAT, 0666)) < 0)
			report_and_exit("open failed...");

		if (fcntl(fd, F_SETLKW, &lock) < 0)
			report_and_exit("fcntl failed to get lock...");
		else {
			write(fd, text, sizeof(text));
			printf(
					"[%d:C][%02d:%02d:%02d][%s] (Trying to send message:”%s” to the Server.)\n",
					getpid(), tm.tm_hour, tm.tm_min, tm.tm_sec, name, text);
		}

		lock.l_type = F_UNLCK;
		if (fcntl(fd, F_SETLK, &lock) < 0)
			report_and_exit("explicit unlocking failed...");

		close(fd);
	}

	return 0;
}
