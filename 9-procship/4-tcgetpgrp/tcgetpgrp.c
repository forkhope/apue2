#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

/* We need a way to tell the kernel which process group is the foreground
 * process group, so that the terminal device driver knows where to send
 * the terminal input and the terminal-generated signals.
 * #include <unistd.h>
 * pid_t tcgetpgrp(int filedes);
 *  Returns: process group ID of foreground process group if OK, -1 on error
 * int tcsetpgrp(int filedes, pid_t pgrpid);
 * 	Returns: 0 if OK, -1 on error.
 * 
 * The function tcgetpgrp() returns the process group ID of the foreground
 * process group associated with the terminal open on filedes.
 *
 * If the process has a controlling terminal, the process can tell tcsetpgrp
 * to set the foreground process group ID to pgrpid. The value of pgrpid
 * must be the process group ID of a process group in the same sessioni, and
 * filedes must refer to the controlling terminal of the session.
 */
int main(void)
{
	pid_t pgid, pid;
	int fd;

	pid = getpid();
	pgid = getpgrp();
	printf("pid = %d, pgid = %d\n", pid, pgid);

	/* 书中9.6小节提到: The way a program guarantees that it is talking to
	 * the controlling terminal is to open the file /dev/tty. This special
	 * file is a synonym within the kernel for the controlling terminal.
	 * 下面打开这个文件,来得到对应控制终端的文件描述符.tcgetpgrp()函数用到
	 * 这个文件描述符.
	 */
	if ((fd = open("/dev/tty", O_RDWR)) < 0) {
		printf("open /dev/tty error: %m\n");
		return 1;
	}

	/* 使用上面得到的文件描述符作为参数来调用tcgetpgrp()函数,从而返回该文件
	 * 描述符所对应控制终端的前台进程组组长ID.
	 */
	if ((pgid = tcgetpgrp(fd)) < 0) {
		printf("tcgetpgrp error: %m\n");
		return 1;
	}
	printf("foreground process group ID = %d\n", pgid);

	pgid = getpgrp();	/* 获取当前进程所在进程组的组长ID */
	if (tcsetpgrp(fd, pgid) < 0) { /* 改变控制终端的前台进程组的组长ID */
		printf("tcsetpgrp error: %m\n");
		return 1;
	}

	/* 再次查询控制终端的前台进程组组长ID,看是否已经改变了 */
	if ((pgid = tcgetpgrp(fd)) < 0) {
		printf("tcgetpgrp error: %m\n");
		return 1;
	}
	printf("After tcsetpgrp(), foreground process group ID = %d\n", pgid);

	return 0;
}
