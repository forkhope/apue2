#include <stdio.h>
#include <signal.h>

static void sig_quit(int signum)
{
	printf("RECEIVED SIGQUIT\n");
}

/* Ths sigpending() function returns the set of signals that are blocked
 * delivery and currently pending for the calling process. The set of
 * signals is returned through the set argument.
 * #include <signal.h>
 * int sigpending(sigset_t *set);
 * 		Returns: 0 if OK, -1 on error
 */
int main(void)
{
	sigset_t newmask, oldmask, pendmask;

	if (signal(SIGQUIT, sig_quit) == SIG_ERR)
		return 1;

	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);

	/* Note that we saved the old mask when we blocked the signal. To
	 * unblock the signal, we did a SIG_SETMASK of the old mask.
	 * Alternatively, we could SIG_UNBLOCK only the signal that we had
	 * blocked. Be aware, however, if we write a function that can be called
	 * by others and if we need to block a signal in our function, we can't
	 * use SIG_UNBLOCK to unblock the signal. In this case, we have to use
	 * SIG_SETMASK and reset the signal mask to its prior value, because
	 * it's possible that the caller had specifically blocked this signal
	 * before calling our function.
	 */
	if (sigprocmask(SIG_SETMASK, &newmask, &oldmask) < 0)
		return 1;

	/* 生成多次 SIGQUIT 信号,但是取消对 SIGQUIT 的阻塞后,进程只接到一次
	 * SIGQUIT 信号,说明了在这个系统中,信号没有排队.即信号如果在阻塞过程
	 * 中生成多次,取消阻塞后,只被递送一次.
	 *
	 * 书中在描述早期 signal() 函数的缺陷时提到,早期的signal()函数在调用过
	 * 一次信号处理函数后,会将信号的处理行为重置为默认(SIG_DFL).
	 * 在Linux下,根据使用的 C函数库 不同, signal()函数的行为也有所不同,查看
	 * man signal 手册的 Portability 小节,可以发现如果使用的glibc 2或者更高
	 * 版本,且定义了_BSD_SOURCE特性宏,则此时的 signal() 函数不会在调用过一次
	 * 信号处理函数后将信号的处理行为重置为默认.
	 * 自己的机子上glibc版本号为: glibc-2.11-1. 经测试发现,在调用 signal()
	 * 函数时,它不会将信号的处理行为重置为默认.
	 *
	 * 这里描述这一点,是为了证明,下面的三个SIGQUIT信号确实是在取消阻塞后,只
	 * 产生了一次,执行结果只能看到一次sig_quit()函数打印出来的字符串.并不是
	 * 说下面的三个SIGQUI信号其实在取消阻塞后产生了三次,但是由于signal()函
	 * 数在执行一次信号处理函数后将信号的处理行为重置为默认,所以执行结果只
	 * 能看到一次sig_quit()函数打印出来的字符串.
	 *
	 * 程序后面在取消对SIGQUIT信号的阻塞后,也生成了三次SIGQUIT信号,可以看到
	 * 执行结果中打印了三次sig_quit()函数中的字符串,符合上面的描述.
	 */
	raise(SIGQUIT);
	raise(SIGQUIT);
	raise(SIGQUIT);

	/* 获取进程当前悬挂的信号集合 */
	if (sigpending(&pendmask) < 0)
		return 1;
	if (sigismember(&pendmask, SIGQUIT) == 1)
		printf("SIGQUIT pending\n");

	/* 恢复原先的信号掩码 */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		return 1;
	printf("SIGQUIT UNBLOCKED\n");

	/* 为了证明 signal() 函数没有在执行过一次信号处理函数就将信号的处理行为
	 * 重置为默认,下面生成了三次SIGQUIT信号,执行结果也显示,sig_quit()函数被
	 * 执行了三次.上面所言非虚.
	 */
	printf("1:\n");
	raise(SIGQUIT);
	printf("2:\n");
	raise(SIGQUIT);
	printf("3:\n");
	raise(SIGQUIT);

	return 0;
}
