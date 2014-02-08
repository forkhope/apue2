#include <stdio.h>
#include <errno.h>
#include <signal.h>

static void pr_mask(const char *str)
{
	sigset_t oset;
	int errno_save;

	errno_save = errno;
	if (sigprocmask(0, NULL, &oset) < 0)
		return;

	printf("%s", str);
	if (sigismember(&oset, SIGINT) == 1)
		printf("SIGINT ");
	if (sigismember(&oset, SIGUSR1) == 1)
		printf("SIGUSR1 ");

	printf("\n");
	errno = errno_save;
}

static void sig_int(int signo)
{
	pr_mask("in sig_int(): ");
}

/* We need a way to both reset the signal mask and put the process to
 * sleep in a single atomic operation. This feature is provided by the
 * sigsuspend() function.
 * #include <signal.h>
 * int sigsuspend(const sigset_t *sigmask);
 * 		Returns: -1 with errno set to EINTR
 * The signal mask of the process is set to the value pointed to by sigmask.
 * Then the process is suspended until a signal is caught or until a signal
 * occurs that terminates the process. If a signal is caught and if the
 * signal handler returns, then sigsuspend() returns, and the signal mask
 * of the process is set to its value before the call to sigsuspend().
 *
 * 这个程序是要在执行某段临界代码之前,先阻塞某个信号,避免这个信号生成时,打
 * 断要执行的临界代码.执行完这段临界代码后,调用sigsuspend()函数解除对该信号
 * 的阻塞,且陷入休眠,等待该信号,或者其他信号的到来.
 */
int main(void)
{
	sigset_t oldmask, newmask, waitmask;
	struct sigaction act;

	/* 进程默认的信号掩码是空,这里打印的出来就是空 */
	pr_mask("program start: ");

	act.sa_handler = sig_int;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGINT, &act, NULL) < 0)
		return 1;

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGINT);
	sigemptyset(&waitmask);
	sigaddset(&waitmask, SIGUSR1);

	/* Block SIGINT and save current signal mask. */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		return 1;

	/* 给自身发送一个 SIGINT 信号,由于该信号此时被阻塞着,所以该信号对应的
	 * 信号处理函数没有被立即执行.
	 */
	raise(SIGINT);

	/* Critical region of code.
	 * 上面将 SIGINT 添加到进程的信号掩码中,所以会打印出来 SIGINT.
	 */
	pr_mask("in critical region: ");

	/* Pause, allowing all signals except SIGUSR1.
	 * 将进程的信号掩码设为 waitmask 的值,即SIGUSR1. 此时, SIGINT 将不存在
	 * 于进程的信号掩码中,所以上面的 raise() 函数生成的 SIGINT 信号会被立刻
	 * 递送,然后 sig_int() 函数被执行,此时打印出来的信号掩码是 SIGINT 和
	 * SIGUSR1. 注意会打印出 SIGINT 是因为系统会自动阻塞当前正在处理的信号.
	 * 虽然上面调用sigprocmask()函数阻塞了SIGINT信号,但是该阻塞在执行下面的
	 * sigsuspend()函数时已经被解除,之所以会打印出SIGINT信号,原因正如上所述.
	 */
	sigsuspend(&waitmask);

	/* 这里打印出来的信号掩码中将不包含 SIGUSR1. Note that when sigsuspend
	 * returns, it sets the signal mask to its value before the call.
	 * 这里会打印出来SIGINT, 这个SIGINT 正是因为上面调用sigprocmask()阻塞了
	 * 该信号,才会被打印出来的. 也就是说,这里和sig_int()函数虽然都打印出来
	 * SIGINT信号,但是SIGINT在这两个地方阻塞的原因是不同的.
	 */
	pr_mask("after return from suspend: ");

	/* Reset signal mask which unblocks SIGINT. */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		return 1;

	pr_mask("program exit: ");
	return 0;
}
