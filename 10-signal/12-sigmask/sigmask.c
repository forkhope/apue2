#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static void pr_mask(const char *str);
static void sig_handler(int signum);

/* The signal mask of a process is the set of signals currently blocked
 * from delivery to that process. A process can examine its signal mask,
 * change its signal mask, or perform both operations in one step by
 * calling the following function.
 * #include <signal.h>
 * int sigprocmask(int how, const sigset_t *restrict set,
 * 				   sigset_t *restrict oset);
 * 		Returns: 0 if OK, -1 on error.
 *
 * First, if oset is a non-null pointer, the current signal mask for the
 * process is returned through oset.
 * Second, if set is a non-null pointer, the how argument indicates how the
 * current signal mask is modified. Note that SIGKILL and SIGSTOP can't be
 * blocked. 下面列举了 how 的不同值以及它们代表的意义.
 * SIG_BLOCK: The new signal mask for the process is the union of its
 *		current signal mask and the signal set pointed to by set. That is,
 *		set contains the additional signals that we want to block.
 * SIG_UNBLOCK: The new signal mask for the process is the intersection of
 * 		its current signal mask and complement of the signal set pointed to
 * 		by set. That is, set contains the signals that we want to unblock.
 * 注意:intersection 是指 "交集",就是以前高中学的集合中的交集,两个集合所共
 * 		有的部分; complement 是指 "补集",设S是一个集合,A是S的一个子集,由S
 * 		中所有不属于A的元素组成的集合,叫做S中子集A的补集.
 * SIG_SETMASK: The new sigal mask for the process is replaced by the value
 * 		of the signal set pointed to by set.
 *
 * If set is a null pointer, the signal mask of the process is not changed,
 * and how is ignored. 此时,可以执行第三个参数 oset 来获取当前的信号掩码.
 *
 * After calling sigprocmask, if any unblocked signals are pending, at
 * least of these signals is delivered to the process before sigprocmask()
 * returns. The sigprocmask() function is defined only for single-threaded
 * process. A separate function is provided to manipulate a thread's
 * signal mask in a multithreaded process.
 */
int main(void)
{
	sigset_t set, set2;

	/* 初始化清空信号集合set,添加 SIGALRM,SIGINT 信号到set中,然后使用
	 * SIG_BLOCK 作为参数调用 sigprocmask() 函数,将这两个信号加入到进程
	 * 的信号掩码中,最后调用 pr_mask() 函数打印进程当前的信号掩码.
	 */
	sigemptyset(&set);
	sigaddset(&set, SIGALRM);
	sigaddset(&set, SIGINT);
	sigprocmask(SIG_BLOCK, &set, NULL);
	pr_mask("SIG_BLOCK: add SIGALRM, SIGINT");

	/* 捕抓 SIGALRM,SIGINT 信号,并调用 raise() 函数给进程本身发送这两个
	 * 信号.通过执行结果可以看到,由于这两个信号已经被阻塞了,它们对应的
	 * 信号处理函数并没有在此刻就得到执行.
	 */
	signal(SIGALRM, sig_handler);
	raise(SIGALRM);
	signal(SIGINT, sig_handler);
	raise(SIGINT);

	/* 添加 SIGUSR1 到信号集合 set2 中,然后使用 SIG_BLOCK 作为参数调用
	 * sigprocmask() 函数,将 SIGUSR1 添加到进程的信号掩码中,最后同样调用
	 * pr_mask() 函数打印进程的信号掩码.由执行结果可见, SIG_BLOCK 只添加新
	 * 的信号到信号掩码中,信号掩码中原有的信号也依然存在,这个 SIG_SETMASK
	 * 参数不同, SIG_SETMASK 参数会用新的信号掩码替换原先的信号掩码.
	 */ 
	sigemptyset(&set2);
	sigaddset(&set2, SIGUSR1);
	sigprocmask(SIG_BLOCK, &set2, NULL);
	pr_mask("SIG_BLOCK: add SIGUSR1");

	/* SIG_SETMASK使用新的信号掩码替换原来的信号掩码,则存在于原来的信号掩码,
	 * 但不存在于新信号掩码中的信号将不再被阻塞,如果此时刚好有这样的信号阻塞
	 * 着,则该信号(或者其他也正在阻塞的信号)会在sigprocmask()返回之前被递送.
	 * 在这个程序中,上面正好阻塞着SIGALRM和SIGINT,所以执行下面的sigprocmask
	 * 函数后,这两个信号不再被阻塞且被递送,它们对应的信号处理函数得以执行.
	 * 所以执行时,会先打印sig_handler()中的字符串,再打印下面pr_mask()中
	 * 的字符串.执行结果中,SIGALRM和SIGINT都先被递送.但书中提到的是 "at
	 * least one of these signals is delivered to the process before
	 * sigprocmask() returns.", 只说是"至少",没说"全部"都会被递送.
	 *
	 * 根据这个描述,执行如下语句后,将会清空进程的信号掩码:
	 * sigemptyset(&set);
	 * sigprocmask(SIG_SETMASK, &set, NULL);
	 */
	sigprocmask(SIG_SETMASK, &set2, NULL);
	pr_mask("SIG_SETMASK, mask the SIGUSR1");

	/* 再次添加 SIGALRM, SIGINT 到信号掩码中,然后使用 SIG_UNBLOCK 作为参数
	 * 调用 sigprocmask() 函数,从信号掩码中移除 SIGUSR1 信号.这个参数只移除
	 * 执定信号集合set中的信号,信号掩码中其他不包含在set的信号会依然存在于
	 * 进程的信号掩码中.
	 */
	sigprocmask(SIG_BLOCK, &set, NULL);
	pr_mask("SIG_BLOCK: add SIGALRM, SIGINT");
	sigprocmask(SIG_UNBLOCK, &set2, NULL);
	pr_mask("SIG_UNBLOCK: remove SIGUSR1");

	/* 使用 SIG_SETMASK 参数和一个空的信号集合清空进程的信号掩码 */
	sigemptyset(&set);
	sigprocmask(SIG_SETMASK, &set, NULL);
	pr_mask("SIG_SETMASK: add a empty set");

	return 0;
}

static void pr_mask(const char *str)
{
	sigset_t oset;
	int errno_save;

	errno_save = errno;	/* we can be called by signal handlers */
	/* 使用下面的语句获取进程当前的信号掩码 */
	if (sigprocmask(SIG_BLOCK, NULL, &oset) < 0)
		exit(1);

	printf("%s: ", str);
	if (sigismember(&oset, SIGALRM) == 1)
		printf("SIGALARM ");
	if (sigismember(&oset, SIGINT) == 1)
		printf("SIGINT ");
	if (sigismember(&oset, SIGQUIT) == 1)
		printf("SIGQUIT ");
	if (sigismember(&oset, SIGUSR1) == 1)
		printf("SIGUSR1");

	/* remaining signals can go here */

	printf("\n");
	errno = errno_save;
}

static void sig_handler(int signum)
{
	printf("RECEIVED SIGNAL: ");
	if (signum == SIGALRM)
		printf("SIGALRM");
	else if (signum == SIGINT)
		printf("SIGINT");
	else
		printf("**UNKNOWN**");
	printf("\n");
}
