#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>
#include <setjmp.h>
#include <signal.h>

static void pr_mask(char *str);
static void sig_usr1(int);
static void sig_usr2(int);
static void sig_alarm(int);
static sigjmp_buf sigenv;
static jmp_buf env;

/* Here, we use the data type sig_atomic_t, which is defined by the ISO C
 * standard to the type of variable that can be written without being
 * interrupted. By this we mean that a variable of this type should not
 * extend across page boundaries on a system with virtual memory and can be
 * accessed with a signal machine instruction, for example. We always
 * include the ISO C qualifier volatile for these data types too, since the
 * variable is being accessed by two different threads of control: the main
 * function and the asynchronously executing signal handler.
 */
static volatile sig_atomic_t canjump;
static volatile sig_atomic_t canjump2;

/* These two functions (sigsetjmp(), siglongjmp()) should always be usred
 * when branching from a signal handler.
 * #include <setjmp.h>
 * int sigsetjmp(sigjmp_buf, env, int savemask);
 * 		Returns: 0 if called directly, nonzero if returning from a 
 * 				 call to siglongjmp()
 * void siglongjmp(sigjmp_buf env, int val);
 *
 * The only difference between these functions and the setjmp and longjmp
 * functions is that sigsetjmp has an additional argument. If savemask is
 * nonzero, then sigsetjmp also saves the current signal mask of the process
 * in env. When siglongjmp is called, if the env argument was saved by a
 * call to sigsetjmp with a nonzero savemask, then siglongjmp restores the
 * saved signal mask.
 *
 * 在 Linux 系统中, setjmp() 和 longjmp() 不会保存和恢复信号掩码.如下所示.
 */
int main(void)
{
	struct sigaction act;

	act.sa_handler = sig_usr1;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGUSR1, &act, NULL) < 0)
		return 1;

	act.sa_handler = sig_usr2;
	if (sigaction(SIGUSR2, &act, NULL) < 0)
		return 1;

	act.sa_handler = sig_alarm;
	if (sigaction(SIGALRM, &act, NULL) < 0)
		return 1;

	pr_mask("starting main(): ");

	/* This program demonstrates another technique that should be used
	 * whenever siglongjmp is called from a signal handler. We set the
	 * variable canjump to a nonzero value only after we've called sigsetjmp
	 * This variable is also examined in the signal handler, and siglongjmp
	 * is called only if the flag canjump is nonzero. This provides
	 * protection against the signal handler being called at some earlier
	 * or later time, when the jump buffer isn't initialized by sigsetjmp.
	 * Since a signal can occur at any time, however, we need the added
	 * protection in a signal handler.
	 */
	if (sigsetjmp(sigenv, 1) == 1) {
		/* 从 siglongjmp() 返回后,由于sigsetjmp() 将之前的信号掩码保存在
		 * sigenv中,所以此时,原先的信号掩码会被恢复,下面打印出来当前的信号
		 * 掩码为空.如果没有保存之前的信号掩码的话,应该会打印出 SIGUSR1
		 */
		pr_mask("After siglongjmp(): ");
	}
	canjump = 1;

	if (setjmp(env) == 1) {
		/* 由于信号处理函数被执行时,当前信号会默认被系统阻塞,而我们又是通过
		 * longjmp() 直接返回的,此时被阻塞的当前信号还会存在于进程的信号掩码
		 * 中,因为 setjmp() 和 longjmp() 不会保存和恢复原先的信号掩码.
		 */
		pr_mask("After longjmp(): ");
		return 0;
	}
	canjump2 = 1;

	for ( ; ; )
		pause();
	return 0;
}

static void pr_mask(char *str)
{
	sigset_t oset;
	int errno_save;

	errno_save = errno;
	if (sigprocmask(0, NULL, &oset) < 0) {
		printf("sigprocmask error");
		exit(1);
	}

	printf("%s", str);
	if (sigismember(&oset, SIGUSR1) == 1)
		printf("SIGUSR1 ");
	if (sigismember(&oset, SIGUSR2) == 1)
		printf("SIGUSR2 ");
	if (sigismember(&oset, SIGALRM) == 1)
		printf("SIGALRM ");

	printf("\n");
	errno = errno_save;
}

static void sig_usr1(int signo)
{
	time_t starttime;

	if (canjump == 0)
		return;

	pr_mask("starting sig_usr1: ");
	alarm(3);

	starttime = time(NULL);
	for ( ; ; ) {
		if (time(NULL) > starttime + 5)
			break;
	}
	pr_mask("finish sig_usr1: ");
	
	canjump = 0;
	siglongjmp(sigenv, 1);
}

static void sig_usr2(int signo)
{
	time_t starttime;

	if (canjump2 == 0)
		return;

	printf("=================================\n");
	pr_mask("in sig_usr2: ");
	alarm(3);

	starttime = time(NULL);
	for ( ; ; ) {
		if (time(NULL) > starttime + 5)
			break;
	}
	pr_mask("finish sig_usr2: ");
	
	canjump2 = 0;
	longjmp(env, 1);
}

static void sig_alarm(int signo)
{
	/* 根据执行结果,可以看到,当SIGALRM信号被递送时,SIGALRM会自动被添加到
	 * 进程的信号掩码中,当程序从 sig_alarm() 函数正常退出时, SIGALRM 又会
	 * 被自动从进程的信号掩码中去掉.
	 */
	pr_mask("in sig_alarm(): ");
}
