#include "apue.h"
#include <fcntl.h>

/* fcntl(): Get/set file descriptor flags (cmd = F_GETFD or F_SETFD)
 * F_GETFD	Return the file descriptor flags for filedes as the value of the
 *			function. Currently, only one file descriptor flag is defined:
 *			the FD_CLOEXEC flag, the close-on-exec flag.
 * F_SETFD	Set the file descriptor flags for filedes. The new flag value is
 *			set from the third argument. If the FD_CLOEXEC bit is 0, the
 *			file descriptor whill remain open across an execve, otherwise
 *			it will be closed.
 */
int main(void)
{
	int fd, fd2, fdflag;

	if ((fd = open("tian", O_RDWR | O_CREAT, 0644)) < 0)
		err_sys("fcntl-fd: can't open or create a new file");

	/* 当 cmd 是 F_GETFD 时,第三个参数是可选的,这个参数会被忽略. */
	if ((fdflag = fcntl(fd, F_GETFD, 0)) < 0)
		err_sys("fcntl-fd: can't get the file descriptor flags");
	printf("fdflag = %d, FD_CLOEXEC = %d\n", fdflag, FD_CLOEXEC);

	if ((fd2 = open("tian", O_RDWR | O_CREAT, 0644)) < 0)
		err_sys("fcntl-fd: can't open or create a new file");

	/* 用 FD_CLOEXEC 来设置 fd2 的 file descriptor flag, 这表示要
	 * close-on-exec. 这个标志就是表示要 close-on-exec.
	 */
	if (fcntl(fd2, F_SETFD, FD_CLOEXEC) < 0)
		err_sys("fcntl-fd: can't set the FD_CLOEXEC flag to fd2");
	if ((fdflag = fcntl(fd2, F_GETFD)) < 0)
		err_sys("fcntl-fd: can't get the file descriptor flags");
	printf("fdflag = %d, FD_CLOEXEC = %d\n", fdflag, FD_CLOEXEC);
	
	close(fd);
	close(fd2);
	exit(0);
}
