#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

/* An open file is closed by calling the close() function.
 * #include <unistd.h>
 * int close(int filedes);
 * Return: 0 if OK, -1 on error.
 * When a process terminates, all of its open files are closed
 * automatically by the kernel. Some programs take advantage of this
 * fact and don't explicitly close open files.
 */

int main(void)
{
	int fd;

	if ((fd = open("a.txt", O_RDWR | O_CREAT, 0644)) < 0)
		printf("close: can't create a new file (a.txt): %s\n",
				strerror(errno));
	else
		printf("close: create and open a new file (a.txt) SUCCESS\n");

	if (close(fd) == 0)
		printf("close: close the opened file(a.txt) SUCCESS\n");
	else
		printf("close: can't close the opened file(a.txt): %s\n",
				strerror(errno));

	/* 调用 close(fd) 之后,再对 fd 调用 write(), read() 函数将会报错.
	 * errno 被设为 EBADF, 即 Bad file descriptor.
	 */
	if (write(fd, "tian", 4) != 4) {
		printf("close: can't write to a.txt: %s\n", strerror(errno));
	}

	return 0;
}

