#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* The Single UNIX Specification includes XSI extensions that allow
 * applications to seek and perform I/O atomically.
 * #include <unistd.h>
 * ssize_t pread(int filedes, void *buf, size_t nbytes, off_t offset);
 * Return: number of bytes read, 0 if end of file, -1 on error
 * ssize_t pwrite(int fd, const void *buf, size_t nbytes, off_t offset);
 * Return: number of bytes written if OK, -1 on error.
 *
 * Calling pread() is equivalent to calling lseek() followed by a call
 * read(), with the following exceptions.
 * 1) There is no way to interrupt the two operations using pread().
 * 2) The file pointer is not updated. 即,调用后"current file offset"不变
 * Calling pwrite() is equivalent to calling lseek() followed by a call
 * to write(), with similar exceptions
 */
int main(void)
{
	int fd;
	char buf[24] = {0};
	size_t n;

	if ((fd = open("yu", O_RDWR | O_CREAT, 0644)) < 0) {
		printf("preadaw: can't open or create yu: %s\n", strerror(errno));
		return 1;
	}
	
	n = strlen("tian xia you qing ren");
	printf("调用pwrite()之前,当前文件偏移为:%d\n", lseek(fd, 0, SEEK_CUR));
	if (pwrite(fd, "tian xia you qing ren", n, 0) < 0) {
		printf("preadaw: can't write to yu: %s\n", strerror(errno));
		return 1;
	}
	printf("调用pwrite()之后,当前文件偏移为:%d\n", lseek(fd, 0, SEEK_CUR));
	
	printf("调用pread()之前,当前文件偏移为:%d\n", lseek(fd, 0, SEEK_CUR));
	if (pread(fd, buf, 24, 0) < 0) {
		printf("preadaw: can't read from yu: %s\n", strerror(errno));
		return 1;
	}
	printf("%s\n", buf);
	printf("调用pread()之后,当前文件偏移为:%d\n", lseek(fd, 0, SEEK_CUR));

	if (read(fd, buf, 8) < 0) {
		printf("preadaw: can't read from yu: %s\n", strerror(errno));
		return 1;
	}
	printf("调用read(fd, buf, 8)之后,当前文件偏移为:%d\n",
			lseek(fd, 0, SEEK_CUR));
	buf[8] = '\0';
	printf("%s\n", buf);

	close(fd);
	return 0;
}
