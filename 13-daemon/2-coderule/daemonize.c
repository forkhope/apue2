#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>		/* open(), O_RDWR */
#include <syslog.h>		/* openlog(), syslog() */
#include <sys/stat.h>	/* umask() */
#include <signal.h>		/* sigaction(), SIG_IGN */
#include <errno.h>
#include <sys/resource.h>	/* getrlimit() */
#include <string.h>

/* Some basic rules to coding a daemon prevent unwanted interactions from
 * happending. The function, daemonize, implements them.
 * 1. The first thing to do is call umask() to set the file mode creation
 * mask to 0. 即,清空子进程从父进程继承到的文件创建掩码,确认子进程调用open()
 * 或者creat()函数创建文件时,指定的文件权限就是真正的权限.
 * 2. Call fork() and have the parent exit. This does several things. First,
 * if the daemon was started as a simple shell command, having the parent
 * terminate makes the shell think that the command is done. Second, the
 * child inherits the process group ID of the parent but gets a new process
 * ID, so we're guaranteed that the child is not a process group leader.
 * This is a prerequisite for the call to setsid() that is done next.
 * 3. Call setsid() to create a new session. The process (a) becomes a
 * session leader of a new session, (b) becomes the process group leader of
 * a new process group, and (c) has no controlling terminal.
 * 4. Change the current working directory to the root directory. The
 * current working directory inherited from the parent could be on a mounted
 * file system. Since daemons normally exist until the system is rebooted,
 * if the daemon stays on a mouned file system, that file system cannot be
 * unmouned.
 * 5. Unneeded file descriptors should be closed. This prevents the daemon
 * from holding open any descriptors that is may have inherited from its
 * parent.
 * 6. Some daemons open file descriptors 0, 1, and 2 to /dev/null so that
 * any library routines that try to read from standard input or write to
 * standard output or standard error will have no effect.
 */
void daemonize(const char *cmd)
{
	pid_t pid;
	struct rlimit lim;
	int i, fd0, fd1, fd2;
	struct sigaction sa;

	/* Clear file creation mask. */
	umask(0);

	/* Get Maximum number of file descriptors */
	if (getrlimit(RLIMIT_NOFILE, &lim) < 0) {
		printf("%s: can't get file limit: %s\n", cmd, strerror(errno));
		exit(1);
	}

	/* Become a session leader to lose controlling TTY */
	if ((pid = fork()) < 0) {
		printf("%s: fork error: %s\n", cmd, strerror(errno));
		exit(1);
	}
	else if (pid != 0) 	/* parent */
		exit(0);
	setsid();

	/* Ensure future opens won't allocate controlling TTYs. */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if (sigaction(SIGHUP, &sa, NULL) < 0) {
		printf("can't caught SIGHUP to SIG_IGN: %s\n", strerror(errno));
		exit(1);
	}
	/* 关于这里要fork()两次的原因,书中解释如下:
	 * Under System V-based systems, some people recommend calling fork()
	 * again() at this point (指上面的第三个步骤之后) and having the parent
	 * terminate. The second child continues as the daemon. This guarantees
	 * that the daemon is not a session leader, which prevents it from
	 * acquiring a controlling terminal under the System V rules (Section
	 * 9.6). Alternatively, to avoid acquiring a controlling terminal, be
	 * sure to specify O_NOTTY whenever opening a terminal device.
	 * 其中提到的 Section 9.6 小节里面,有如下的描述:
	 * Ssystems derived from UNIX System V allocate the controlling terminal
	 * for a session when the session leader opens the first terminal device
	 * that is not already associated with a session. This assumes that the
	 * call to open() by the session leader does not specify the O_NOTTY
	 * flag.
	 */
	if ((pid = fork()) < 0) {
		printf("%s: fork again error: %s\n", cmd, strerror(errno));
		exit(1);
	}
	else if (pid != 0)	/* parent */
		exit(0);

	/* Change the current working directory to the root so we don't prevent
	 * file systems from being unmounted.
	 */
	if (chdir("/") < 0) {
		printf("%s: chdir to / error: %s\n", cmd, strerror(errno));
		exit(1);
	}

	/* Close all open file descriptors. */
	if (lim.rlim_max == RLIM_INFINITY)
		lim.rlim_max = 1024;
	for (i = 0; i < lim.rlim_max; ++i)
		close(i);

	/* Attach file descriptors 0, 1, and 2 to /dev/null. */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/* Initialize the log file. */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d %d %d",
				fd0, fd1, fd2);
		exit(1);
	}

	/* 执行sleep(10),让进程睡眠,以便通过ps命令来查看守护进程当前状态 */
	sleep(10);
}

int main(void)
{
	printf("Now, call the daemonize() function!!\n");
	daemonize("now");
	return 0;
}
