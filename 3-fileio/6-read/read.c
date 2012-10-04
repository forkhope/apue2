#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE	4096	/* max line length */

/* Data is read from an open file with the read() function.
 * #include <unistd.h>
 * ssize_t read(int filedes, void *buf, size_t nbytes);
 * Return: number of bytes read, 0 if end of file, -1 on error.
 *
 * The read operation starts at the file's current offset. Before a
 * successful return, the offset is incremented by the number of bytes
 * actually read.
 */
int main(void)
{
	int fd;
	char buf[MAXLINE];
	ssize_t n;

	if ((fd = open("Makefile", O_RDONLY)) < 0) {
		printf("read: can't open the file (Makefile): %m\n");
		return 1;
	}

	/* When reading from a regular file, if the end of file is reached
	 * before the requested number of bytes has been read. For example,
	 * if 30 bytes remain until the end of file and we try to read 100
	 * bytes, read() returns 30. The next time we call read(), it will
	 * return 0 (end of file).
	 */
	while ((n = read(fd, buf, MAXLINE)) > 0)
		if (write(STDOUT_FILENO, buf, n) != n) {
			printf("read: can't write buf to STDOUT: %m\n");
			return 1;
		}
	
	if (n == -1) {
		printf("read: can't read from fd: %m\n");
		return 1;
	}

	close(fd);
	return 0;
}
