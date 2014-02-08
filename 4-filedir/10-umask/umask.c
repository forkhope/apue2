#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>

#define RWRWRW	(S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH)

/* The umask() function sets the file mode creation mask for the process
 * and returns the previous value. (This is one of the few functions that
 * doesn't have an error return.)
 * #include <sys/stat.h>
 * mode_t umask(mode_t cmask);
 *		returns: previous file mode creation mask.
 * The cmask arguments is formed as the bitwise OR of any of the nine
 * constants form: S_IRUSR, S_IWUSR, and so on.
 * 
 * The file mode creation mask is used whenever the process creates a new
 * file or a new directory. Any bits that are on in the file mode creation
 * mask are turned off in the file's mode.
 * 在调用 open() 或者 creat() 函数新建文件时,需要指定新文件的权限,但是这个
 * 权限并不一定就是新文件真正的权限,指定的权限值需要和某个值相与,得到的值
 * 才是新文件真正的权限.这个值就是对文件创建掩码(file mode creation mask)
 * 求反之后得到的值. 具体的例子见下面的程序:
 *
 * If we want to ensure that anyone can read a file, we should set the
 * umask to 0.
 */
int main(int argc, char *argv[])
{
	/* 将文件创建掩码设为 0066, 对掩码求反得到 0711, 可见组用户的读写权限,
	 * 其他用户的读写权限被掩盖了.下面的 creat() 函数指定的权限值为 0666,
	 * 本来是打算创建一个user, group, other都可读写的文件. 0666 和 0711 相
	 * 与得到 0600, 最终创建的文件会失去group, other的读写权限.
	 */
	umask(S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
	if (creat("bar", RWRWRW) < 0) {
		printf("%s: creat bar error:%s\n",argv[0],strerror(errno));
		exit(EXIT_FAILURE);
	}

	/* 将文件的创建掩码设为 0, 求反之后得到 0777,即不会掩盖任何的权限位,
	 * 此时, creat() 函数指定的权限就是所创建文件的真正权限.
	 */
	umask(0);
	if (creat("foo", RWRWRW) < 0) {
		printf("%s: creat foo error:%s\n",argv[0],strerror(errno));
		exit(EXIT_FAILURE);
	}

	exit(EXIT_SUCCESS);
}
