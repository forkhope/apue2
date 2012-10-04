#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

/* A new file can also be created by calling the creat() function.
 * #include <fcntl.h>
 * int creat(const char *pathname, mode_t mode);
 * Return: file descriptor opened for write-only if OK, -1 on error.
 * Note that this function is equivalent to:
 *		open(pathname, O_WRONLY | O_CREAT | O_TRUNC, mode);
 * One deficiency with creat() is that the file is opened only for writing
 * With the O_CREAT and O_TRUNC options now provided by open(), a separate
 * creat() function is no longer needed.
 */

int main(void)
{
	int fd;
	char buf[10];

	if ((fd = creat("a.txt", 0644)) < 0)
		/* %m 是 glibc 的扩展转义,相当于 "%s", strerror(errno) */
		printf("creat: can't create a new file(a.txt) %m\n");
	else
		printf("creat: create a new file(a.txt) SUCCESS\n");

	if (read(fd, buf, 10) < 0)
		printf("creat: can't read from a.txt, since write-only\n");

	close(fd);
	return 0;
}
