#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* Every process has a current working directory. This directory is where
 * the search for all relative pathnames starts (all pathnames that do not
 * begin with a slash). When a user logs in to a UNIX system, the current
 * working directory normally starts at the directory specified by the sixth
 * field in the /etc/passwd file--the user's home directory. The current
 * working directory is an attribute of a process; the home directory is
 * an attribute of a login name.
 * We can change the current working directory of the calling process by
 * calling the chdir() or fchdir() functions.
 * #include <unistd.h>
 * int chdir(const char *pathname);
 * int fchdir(int filedes);
 * 		Both return: 0 if OK, -1 on error.
 * We can specify the new current working directory either as a pathname
 * or through an open file descriptor.
 *
 * Because it is an attribute of a process, the current working directory
 * cannot affect processes that invoke the process that executes the
 * chdir().即 chdir() 函数只影响本身进程,影响不到它的父进程.
 */
int main(int argc, char *argv[])
{
	int fd;

	if (argc != 2) {
		printf("usage: %s <dir>\n", argv[0]);
		return 1;
	}

	printf("Now, the current working directory is:\n");
	system("pwd");

	printf("Before chdir, open the current directory and sava fd\n");
	// 下面的open()函数打开一个目录,由于只有内核可以直接写一个目录,所以
	// 指定的权限不能带有写权限,否则可以编译通过,但是运行报错,errno被
	// 置为 EISDIR, 对应的描述字符串是: Is a directory
	if ((fd = open(".", O_RDONLY)) < 0) {
		printf("open error: %s\n", strerror(errno));
		return 1;
	}

	if (chdir(argv[1]) < 0) {
		printf("chdir %s error: %s\n", argv[1], strerror(errno));
		return 1;
	}

	printf("After chdir, the current working directory is: \n");
	system("pwd");

	printf("Execute the ls command in the current working directory:\n");
	system("ls");

	printf("After execute ls command, return to the last directory\n");
	if (fchdir(fd) < 0) {
		printf("fchdir error: %s\n", strerror(errno));
		return 1;
	}

	printf("After fchdir, the current working directory is:\n");
	system("pwd");

	return 0;
}
