#include <stdio.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

typedef void Sigfunc(int);
static Sigfunc *Signal(int signum, Sigfunc *func);
static void sig_alarm(int signum);

/* The sigaction() function allows us to examine or modify (or both) the
 * action associated with a particular signal. This function supersedes the
 * signal() function from earlier releases of the UNIX System.
 * #include <signal.h>
 * int sigaction(int signo, const struct sigaction *restrict act,
 * 				 struct sigaction *restrict oact);
 * 				 		Returns: 0 if OK, -1 on error
 * The argument signo is the signal number whose action we are examing or
 * modifying. If the act pointer is non-null, we are modifying the action.
 * If the oact pointer is non-null, the system returns the previous action
 * for the signal through the oact pointer. The struct sigaction is like:
 * 	struct sigaction {
 * 		void (*sa_handler)(int);	// address of signal handler
 * 									// or SIG_IGN, SIG_DFL
 *		sigset_t sa_mask;			// additional signals to block
 *		int sa_flags;				// signal options
 *		// alternate handler
 *		void (*sa_sigaction)(int, siginfo_t *, void *);
 *	};
 *
 * 书中在描述 fa_flags 时有个迷惑的地方,描述如下:
 * SA_NOCLDSTOP: If signo is SIGCHLD, do not generate this signal when a
 * 				 child process stops (job control). This signal is still
 * 				 generated, or course, when a child terminates。 As an XSI
 * 				 extension, SIGCHLD won't be sent when a stopped child
 * 				 continues if this flag is set.
 * 这个描述提到:作为XSI扩展,当设置SA_NOCLDSTOP标志被设置时,暂停的子进程再次
 * 被继续执行后将不会发送SIGCHLD信号.按照这个意思,正常情况下,暂停的子进程被
 * 继续执行后会发送SIGCHLD信号,但是书中10.2小节在描述SIGCHLD时,只提到当进程
 * 终止或者暂停时,会发送SIGCHLD信号;在书中其他地方也没有见到提到过暂停的子进
 * 程被继续执行后会发送SIGCHLD信号.这应该是书中的疏漏吧.查阅 Open Group 的
 * 在线文档,里面确实提到,XSI有个扩展是,暂停的子进程被继续执行后会发送SIGCHLD
 * 信号,网址是:http://pubs.opengroup.org/onlinepubs/9699919799/
 * 两者对照,书中对SA_NOCLDSTOP的描述就好理解了.
 *
 * Once we install an action for a given signal, that action remains
 * installed until we explicitly change it by calling sigaction(). Unlike
 * earlier systems with their unreliable signals, POSIX.1 requires that a
 * signal handler remain installed until explicitly changed.
 */
int main(void)
{
	char buf[BUFSIZ];
	int n;

	if (Signal(SIGALRM, sig_alarm) == SIG_ERR)
		return 1;

	/* 在 apue2/10-signal/10-alrmread/alarmread.c文件中,提到自己的Linux系统
	 * 中,read()函数会中断自动重入,导致无法使用alarm()函数来提前退出read()
	 * 函数,当时使用的是系统提供的signal()函数.这里使用 sigaction()函数以及
	 * SA_INTERRUPT标志来再次实现这个功能,执行结果和预期一致,当3秒后,还没有
	 * 接收到输入时, read()函数报错返回,且 errno 被置为 EINTER.
	 */
	alarm(3);
	if ((n = read(STDIN_FILENO, buf, BUFSIZ)) < 0) {
		if (errno == EINTR)
			printf("read, errno == EINTR\n");
		else
			printf("read, errno != EINTR\n");
	}

	buf[n] = '\0';
	printf("READ: %s", buf);
	return 0;
}

/* Reliable version of signal(), using POSIX sigaction() */
static Sigfunc *Signal(int signum, Sigfunc func)
{
	struct sigaction act, oact;

	act.sa_handler = func;

	/* 书中特别提到不能使用 act._sa_mask = 0; 来初始化 sa_mask 成员.
	 * Note that we must use sigemptyset() to initialize the sa_mask member
	 * of the structure. We're not guaranteed that "act.sa_mask = 0;" does
	 * the same thing.
	 *
	 * The sa_mask field specifies a set of signals that are added to the
	 * signal mask of the process before the signal-catching function is
	 * called. If and when the signal-catching function returns, the signal
	 * mask of the process is reset to its previous value.
	 */
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if (signum == SIGALRM) {
#ifdef SA_INTERRUPT
		printf("define the SA_INTERRUPT\n");
		act.sa_flags |= SA_INTERRUPT;
#endif
	}
	else {
#ifdef SA_RESTART
		printf("define the SA_RESTART\n");
		act.sa_flags |= SA_RESTART;
#endif
	}
	if (sigaction(signum, &act, &oact) < 0)
		return SIG_ERR;
	return oact.sa_handler;
}

static void sig_alarm(int signum)
{
	sigset_t oact;

	/* The operating system includes the signal being delivered in the
	 * signal mask when the handler is invoked. Hence, we are guaranteed
	 * that whenever we are processing a given signal, another occurrence
	 * of that signal is blocked until we're finished processing the first
	 * occurrence. Recall from Section 10.8 that additional occurrences of
	 * the same signal are usually not queued. If the signal occurs five
	 * times while it is blocked, when we unblock the signal, the
	 * signal-handling function for that signal will usuallly be invoked
	 * only one time.
	 * 当某个信号对应的信号处理函数正在执行时,操作系统会自动添加该信号到当
	 * 前进程的信号掩码中,以阻塞该信哈的递送,直到此时正在执行的信号处理函数
	 * 执行结束后,才取消该阻塞.如果在阻塞期间,该信号产生多次,由于UNIX系统一
	 * 般不对信号排队,则取消阻塞后,该信号只被递送一次.
	 */
	if (sigprocmask(0, NULL, &oact) < 0)
		return;

	if (sigismember(&oact, SIGALRM) == 1)
		printf("The signal SIGALRM is in the signal mask\n");
	else
		printf("The signal SIGALRM isn't in the signal mask\n");
}
