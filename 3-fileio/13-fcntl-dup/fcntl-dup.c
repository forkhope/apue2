#include "apue.h"
#include <fcntl.h>

/* The fcntl() function can change the properties of a file that is already
 * open.	#include <fcntl.h>
 * int fcntl(int filedes, int cmd, ... // arg );
 * 如果 cmd 等于 F_DUPFD, 则fcntl()函数的功能是复制一个已存在的文件描述符.
 * F_DUPFD	Duplicate the file descriptor filedes. The new file descriptor
 * is returned as the value of the function. It is the lowest-numbered
 * descriptor that is not already open, that is greater than or equal to
 * the third argument (taken as an integer). The new descriptor shares the
 * same file table entry as filedes. But the new descriptor has its own set
 * of file descriptor flags, and its FD_CLOEXEC file descriptor flag in
 * cleared.此时,该函数的返回值是复制之后的新文件描述符的值.
 *
 * 使用 fcntl() 函数复制文件描述符,可以提供一个最小值,虽然依然无法确切知道
 * 具体的值,但可以确定该值一定大于或等于所给的第三个参数;而 dup() 函数无法
 * 确定任何可能的值(说不定可以猜出来); dup2()函数比 fcntl()函数更近一步,它
 * 可以指定复制之后的文件描述符的值.
 */
int main(void)
{
	int fd, fd2, fd3;

	/* Indeed, the call "dup(filedes);" is equivalent to "fcntl(filedes,
	 * F_DUPFD, 0);". dup()函数从进程文件描述符表里面所有没有使用的文件描述
	 * 符中取出最小的那个,作为返回值返回; 当cmd等于 F_DUPFD 时, fcntl()函数
	 * 的返回值大于或等于第三个参数的值,所以前面要将第三个参数的值设为0,表
	 * 示从0开始寻找还没有使用的文件描述符,即查找所有还没有使用的文件描述符
	 */
	if ((fd = fcntl(1, F_DUPFD, 0)) < 0)
		err_sys("fcntl-dup: fcntl error");
	printf("fd = %d\n", fd);

	/* 此时, fd 指向标准输出文件,对 fd 写入,将会在标准输出上显示出来 */
	if (write(fd, "tian\n", 5) != 5)
		err_sys("fcntl-dup: can't write to fd: %d", fd);

	/* 当 cmd = F_DUPFD 时, fcntl()返回的新文件描述符值大于或等于第三个参数
	 * 的值,由于此时进程中还没有使用过 6 这个文件描述符,下面的fcntl()函数将
	 * 会返回 6.这样,程序可以近似的指定出fcntl()函数的返回值. "dup2(filedes,
	 * filedes2);" 类似于 close(filedes2); fcntl(filedes, F_DUPFD, filedes2)
	 * 当然,这只是类似,书中 3.12 小节描述了这两者之间的区别.
	 */
	if ((fd2 = fcntl(fd, F_DUPFD, 6)) < 0)
		err_sys("fcntl-dup: fcntl error");
	printf("fd2 = %d\n", fd2);

	// 由于执行上面的语句后, 6 这个文件描述符已经在使用, fd3 的值将会是 7
	if ((fd3 = fcntl(fd, F_DUPFD, 6)) < 0)
		err_sys("fcntl-dup: fchtl error");
	printf("fd3 = %d\n", fd3);

	close(fd);
	close(fd2);
	close(fd3);
	exit(0);
}
