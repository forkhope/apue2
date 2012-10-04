#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* Because the open() function follows a symbolic link, we need a way to
 * open the link itself and read the name in the link. The readlink()
 * function does this.
 * #include <unistd.h>
 * ssize_t readlink(const char *restrict pathname, char *restrict buf,
 * 					size_t bufsize);
 * 			Returns: number of bytes read if OK, -1 on error.
 * This function combines the actions of open(), read() and close(). If
 * the function is successful, it returns the number of bytes placed into
 * buf. The contents of the symbolic link that are returned in buf are
 * not null terminated.
 *
 * 据 man手册 和 实际的测试结果可以知道,当传入的文件不是 symbolic link 时,
 * readlink()函数会报错,返回-1,并将 errno 设为 EINVAL.
 */
int main(int argc, char *argv[])
{
	char buf[BUFSIZ];
	int n;

	if (argc != 2) {
		printf("Usage: %s filename\n", argv[0]);
		return 1;
	}

	if ((n = readlink(argv[1], buf, BUFSIZ-1)) < 0) {
		printf("readlink %s error: %s\n", argv[1], strerror(errno));
		return 1;
	}
	printf("read %d bytes\n", n);

	buf[BUFSIZ-1] = '\0';
	printf("%s link to: %s\n", argv[1], buf);
	return 0;
}
