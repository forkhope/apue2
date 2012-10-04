#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	int fd, i;

	if ((fd = open("yu", O_RDWR | O_CREAT, 0644)) < 0) {
		printf("sharing b: can't open or create yu:%s\n",strerror(errno));
		return 1;
	}

	for (i = 0; i < 3; ++i) {
		if (write(fd, "TI", 2) != 2) {
			printf("sharing b: can't write to yu:%s\n",strerror(errno));
			return 1;
		}
		sleep(1);
	}
	close(fd);

	printf("sharing b: finished");
	return 0;
}
