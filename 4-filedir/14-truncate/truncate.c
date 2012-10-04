#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>

/* There are times when we would like to truncate a file by chopping off
 * data at the end of the file. Emptying a file, which we can do with the
 * O_TRUNC flag to open(), is a special case of truncation.
 * #include <unistd.h>
 * int truncate(const char *pathname, off_t length);
 * int ftruncate(int filedes, off_t length);
 *		Both return: 0 if OK, -1 on error.
 *
 * These two functions truncate an existing file to length bytes. If the
 * previous size of file was greater than length, the data beyond length is
 * no longer accessible. If the previous size was less than length, the
 * effect is system dependent, but XSI-conforming systems will increase the
 * file size. If the implementation does extend a file, data between the
 * old end of file and the new end of file will read as 0 (i.e., a hole is
 * probably created in the file). 经测试, linux系统就是这种情况.
 * truncate() 函数的第二个参数指定的就是截断后的文件长度.
 */
int main(int argc, char *argv[])
{
	if (argc != 3) {
		printf("Usage: %s <pathname> <length>\n", argv[0]);
		exit(1);
	}

	if (truncate(argv[1], atoll(argv[2])) < 0) {
		printf("%s: truncate %s error: %s\n", argv[0], argv[1],
				strerror(errno));
		exit(1);
	}

	exit(0);
}
