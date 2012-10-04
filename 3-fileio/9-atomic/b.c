#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

int main(void)
{
	int fd, i;

	/* 根据书中3.11小节的描述,使用 O_APPEND 标志打开的文件,将会把 "position
	 * to the end of file and write" 作为一个原子操作,在每次写入之前,都会把
	 * "current file offset"设为文件末尾,写入之后,也会增大文件的size,这样,
	 * 即使是多个进程同时读写一个文件,也能保证它们之间的写入操作是交错进行,
	 * 不会造成覆盖的情况. 书中描述了一种会覆盖的情况.
	 */
	if ((fd = open("yu", O_RDWR | O_APPEND | O_CREAT, 0644)) < 0) {
		printf("atomic a: can't open or create yu: %s\n",strerror(errno));
		return 1;
	}
	for (i = 0; i < 3; ++i) {
		if (write(fd, "xia\n", 4) != 4) {
			printf("atomic a: can't write to yu: %s\n",strerror(errno));
			return 1;
		}
		sleep(1);
	}
	close(fd);

	printf("atomic a: finished\n");
	return 0;
}
