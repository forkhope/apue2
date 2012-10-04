#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>

/* getcwd - get current working directory.
 * #include <unistd.h>
 * char *getcwd(char *buf, size_t size);
 * 		Returns: buf if OK, NULL on error.
 *
 * The function returns a null-terminated string containing an absolute
 * pathname that is the current working directory of the calling process.
 * The pathname is returned as the function result and via the argument buf,
 * if present. As an extension to the POSIX.1-2001 standard, Linux getcwd()
 * allocates the buffer dynamically using malloc() if buf is NULL. The
 * caller should free() the returned buffer.
 *
 * The getcwd() function copies an absolute pathname of the current working
 * directory to the array pointed to by buf, which is of length size.
 * If the length of the absolute pathname of the current working directory,
 * including the terminating null byte, exceeds size bytes, NULL is returned
 * and errno is set to ERANGE.
 * 注意,这里所说的 "length of the absolute pathname" 是指 getcwd() 函数返回
 * 的当前工作目录的绝对路径长度,不是Linux系统中路径的最大长度.如果当前工作
 * 目录是 /home/lixianyi, 则其长度为 14, 加上'\0',共 15 个字符,则提供的缓冲
 * 区的长度应该大于或者等于 15,才不会报错,如果小于 15,就会报错.
 */
int main(void)
{
	char buf[BUFSIZ];
	char smallbuf[20];

	if (getcwd(buf, BUFSIZ) < 0) {
		printf("getcwd error: %s\n", strerror(errno));
		return 1;
	}

	printf("The current working directory is:\n");
	printf("%s\n", buf);

	printf("The length of pathname is: %d\n", strlen(buf));
	printf("smallbuf缓冲区的长度为20,getcwd() smallbuf将会报错\n");

	/* 过小的缓冲区会报错 */
	if (getcwd(smallbuf, 20) == NULL)
		printf("getcwd, smallbuf, error: %s\n", strerror(errno));
	return 0;
}
