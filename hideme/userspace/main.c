#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>


void usage(const char *name) {
	printf("USAGE: %s [-h] [-r]\n", name ? name : "");
	printf("\t-h\tPrint usage.\n");
	printf("\t-r\tRemove self from task list.\n");
}

int foo(int bar) {
	if (bar)
		return 1;
	else
		return 0;
}

int main(int argc, char *argv[]) {
	int c;
	bool remove_me = false;
	while ((c = getopt (argc, argv, "hr")) != -1) {
		switch (c) {
			case 'h':
				usage(argv[0]);
				exit(0);
				break;
			case 'r':
				remove_me = true;
				break;
			default:
				break;
		}
	}

	if (remove_me) {
		int fd;
		printf("removing myself from task list: %d\n", getpid());
		fd = open("/dev/hideme", O_RDONLY);
		if (fd < 0) {
			perror("failed to open char device");
			return -1;
		}
		close(fd);
	}

	printf("Press ENTER to exit\n");
	getchar();

	return 0;
}
