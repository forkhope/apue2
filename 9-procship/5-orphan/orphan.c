#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>	/* sleep() */
#include <signal.h>

static void sig_hup(int signum)
{
	printf("SIGHUP received, pid = %d\n", getpid());
}

static void pr_ids(char *name)
{
	printf("%s: pid = %d, ppid = %d, pgrp = %d, tpgrp = %d\n", name,
			getpid(), getppid(), getpgrp(), tcgetpgrp(STDIN_FILENO));
	fflush(stdout);
}

/* The POSIX.1 definition of an orphaned process group is one in which the
 * parent of every member is either itself a member of the group or is not
 * a member of the groups session. Another way of wording this is that the
 * process group is not orphaned as long as a process in the group has a
 * parent in a different process group but in the same session.
 * 即,孤儿进程组是指:该组中每个成员的父进程要么是该组的一个成员,要么不是该组
 * 所属会话的成员.另一种描述是:一个进程组不是孤儿进程组的条件是,该组中有一个
 * 进程,其父进程在属于同一会话的另一个组中.
 * 
 * Since the process group is orphaned when the parent terminates, POSIX.1
 * requires that every process in the newly orphaned process group that is
 * stopped be sent the hang-up signal (SIGHUP) followed by the continue
 * signal (SIGCONT). The default action for the hang-up signal is to
 * terminate the process. If we don't want this, we have to provide a signal
 * handler to catch the signal. 如该程序中所作的这样.
 */
int main(void)
{
	char c;
	pid_t pid;

	pr_ids("parent");
	if ((pid = fork()) < 0)
		exit(1);
	else if (pid > 0) {	/* parent */
		/* The parent sleeps for 5 seconds. This is our (imperfect) way of
		 * letting the child execute before the parent terminates.
		 */
		sleep(5);		/* sleep to let child stop itself */
		exit(0);		/* then parent exits */
	}

	/* child */
	pr_ids("child");
	
	/* The child establishes a signal handler for the hang-up signal(SIGHUP)
	 * This is so we can see whether SIGHUP is sent to the child.
	 */
	signal(SIGHUP, sig_hup);	/* establish signal handler */

	/* The child sends itself the stop signal (SIGTSTP) with kill() function
	 * This stops the child, similar to our stopping a foreground job with
	 * out terminal's suspend character (Control-Z).
	 */
	kill(getpid(), SIGTSTP);	/* stop ourself */

	/* 在子进程暂停期间,父进程休眠5秒之后会退出;当其父进程退出后,子进程被过
	 * 继给init进程,而init进程和shell进程并不在同一个会话里面.根据上面的描述
	 * 此时,该子进程成为孤儿进程组中的一员,由于它此时是暂停的,它将会收到一个
	 * SIGHUP信号,然后再收到一个SIGCONT信号.
	 *
	 * 在该程序中,子进程收到SIGHUP信号后,会执行之前安装的信号处理函数
	 * sig_hup();然后在接收到SIGCONT信号后,继续往下执行,从而执行下面的语句.
	 */
	pr_ids("child");	/* prints only if we're continued */

	/* After calling pr_ids() in the child, the program tries to read from
	 * standard input. As we saw earlier in this chapter, when a background
	 * process group tries to read from its controlling terminal, SIGTTIN
	 * is generated for the background process group. But here we have an
	 * orphaned process group; if the kernel were to stop it with this
	 * signal, the processes in the process group would probably never be
	 * continued. POSIX.1 specifies that the read() is to return an error
	 * with errno set to EIO in this situation.
	 *
	 * Finally, note that our child was placed in a background process
	 * group when the parent terminated, since the parent was executed as
	 * a foreground job by the shell.
	 */
	if (read(STDIN_FILENO, &c, 1) != 1)
		printf("read error from controlling TTY: errno = %d\n", errno);

	exit(0);
}
