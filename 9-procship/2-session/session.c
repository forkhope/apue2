#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

/* A session is a collection of one or more process groups.
 * The processes in a process group are usually placed there by a shell
 * pipeline. 如执行 proc1 | proc2 | proc3 后,这三个进程就是同一个进程组.
 * 
 * A process establishes a new session by calling the setsid() function.
 * #include <unistd.h>
 * pid_t setsid(void);
 * 		Returns: process group ID if OK, -1 on error.
 * If the calling process is not a process group leader, this function
 * creates a new session. Three things happen.
 * 1. The prcoess becomes the session leader of this new session. (A
 * session leader is the process that creates a session.) The process is
 * the only process in this new session.
 * 2. The process becomes the process group leader of a new process group.
 * The new process group ID is the process ID of the calling process.
 * 3. The process has no controlling terminal. If the process had a
 * controlling terminal before calling setsid(), that association is broken.
 *
 * This function returns an error if the caller is already a process group
 * leader.
 */
int main(void)
{
	pid_t pid, pgid;

	pid = getpid();
	pgid = getpgrp();
	printf("Before setsid(): pid = %d, pgid = %d\n", pid, pgid);
	
	/* 直接在 shell 中执行这个程序时,可以看到上面打印的pid和pgid相等,
	 * 此时,下面语句会报错: setsid error: Operation not permitted
	 * 查看 man setsid 手册发现,当一个进程的进程ID等于它的进程组ID时,调用
	 * setsid()函数会报错,errno被置为 EPERM. 这是因为若一个进程的进程ID等于
	 * 它的进程组ID,则该进程是它所在进程组的进程组领导者.上面提到了setsid()
	 * 函数被一个进程组领导者调用时,会执行出错.
	 */
	if ((pgid = setsid()) < 0) {
		printf("setsid error: %s\n", strerror(errno));
		return 1;
	}

	/* 下面打印出来的 pgid 的值等于该进程的进程ID.参见上面描述的第二点.
	 * setsid()函数会新建一个进程组,且新进程组的进程组ID等于该进程的进程ID
	 */
	printf("The pgid returned by setsid(): %d\n", pgid);

	pid = getpid();
	pgid = getpgrp();
	printf("After  setsid(): pid = %d, pgid = %d\n", pid, pgid);

	/* 调用 setsid() 函数后,该进程将不关联控制终端,下面尝试获取标准输入
	 * 对应的前台进程组组长ID时,tcgetpgrp() 函数会报错,errno被置为ENOTTY.
	 */
	if ((pgid = tcgetpgrp(STDIN_FILENO)) < 0) {
		printf("tcgetpgrp error: %s\n", strerror(errno));
	}

	return 0;
}
