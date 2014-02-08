#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define MAXLINE	4096	/* max line length */

/* Data is read from an open file with the read() function.
 * #include <unistd.h>
 * ssize_t read(int filedes, void *buf, size_t nbytes);
 * Return: number of bytes read, 0 if end of file, -1 on error.
 *
 * There are several cases in which the number of bytes actually read is
 * less than the amount requested:
 * 1.When reading from a regular file, if the end of file is reached before
 * the requested number of bytes has been read. For example, if 30 bytes
 * remain until the end of file and we try to read 100 bytes, read returns
 * 30. The next time we call read, it will return 0 (end of file).
 * 2.When reading from a terminal device. Normally, up to one line is read
 * at a time. (We'll see how to change this in Chapter 18.)
 * 3.When reading from a network. Buffering within the network may cause
 * less than the requested amount to be returned.
 * 4.When reading from a pipe or FIFO. If the pipe contains fewer bytes
 * than requested, read will return only what is available.
 * 5.When reading from a record-oriented device. Some record-oriented
 * devices, such as magnetic tape, can return up to a single record at a
 * time.
 * 6.When interrupted by a signal and a partial amount of data has already
 * been read. We discuss this further in Section 10.5.
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
