#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

static void sig_usr(int); /* one handler for both signals */

/* Signals are software interrupts. Signals provide a way of handlings
 * asynchronous events: a user at a terminal typing the interrupt key to
 * stop a program or the next program in a pipeline terminating permaturely.
 *
 * Every signal has a name. These names all begin with the three characters
 * SIG. These names are all defined by positive interger constants (the
 * signal number) in the header <signal.h>.
 *
 * 当进程接收到一个信号时,它可以:
 * (1)Ignore the signal. SIGKILL 和 SIGSTOP 不能被忽略.
 * (2)Catch the signal. 同样, SIGKILL 和 SIGSTOP 不能被捕抓.
 * (3)Let the default action apply. 大部分信号的默认行为是终止该进程.
 *
 * The simplest interface to the signal features of the UNIX System is
 * the signal() function. 这个函数已经不提倡使用.
 * #include <signal.h>
 * void (*signal(int signo, void (*func)))(int);
 * 	Returns: previous disposition of signal if OK, SIG_ERR on error.
 * 这个函数的第二个参数是一个函数指针,该指针的值是 (a)the constant SIG_IGN,
 * (b)the constant SIG_DFL, or (c)the address of a function to be called
 * when the signal occurs. If we specify ISG_IGN, we are telling the system
 * t ignore the signal. When we specify SIG_DFL, we are setting the action
 * associated with the signal to its default value. 若我们指定一个信号处理
 * 函数,该函数被调用时,传入的参数是当前的信号值.
 *
 * When a program is executed, the status of all signals is either default
 * or ignore. Normally, all signals are set to their default action, unless
 * the process that calls exec() is ignoring the signal.
 * Specifically, the exec functions change the disposition of any signals
 * being caught the their default action and leave the status of all other
 * signals alone. (Naturally, a signal that is being caught by a process
 * that calls exec cannot be caught by the same function in the new program,
 * since the address of the signal-catching function in the caller probably
 * has no meaning in the new program file that is executed.)
 * 即,执行exec()函数时,exec()函数将父进程中已经被捕抓的信号的行为设为默认,
 * 保留那些没有被捕抓的信息的行为不变.如果某个信号在父进程中设为忽略,则执行
 * exec()函数后,这个信号的行为还是忽略.
 *
 * When a process calls fork(), the child inherits the parent's signal
 * dispositions. Here, since the child starts off with a copy of the
 * parent's memory image, the address of a signal-catching function has
 * meaning in the child.
 */
int main(void)
{
	/* 下面调用的两次signal()函数都会报错:
	 * signal SIGKILL error: Invalid argument
	 * signal SIGSTOP error: Invalid argument
	 * 可见,使用 SIGKILL, SIGSTOP 作为signal()函数的参数时,会报错.这两个信
	 * 号不可被忽略,不可被捕抓.即使是想用signal()函数设置它们为默认也不行.
	 */
	if (signal(SIGKILL, SIG_IGN) == SIG_ERR)
		printf("signal SIGKILL to SIG_IGN error: %s\n", strerror(errno));
	if (signal(SIGSTOP, sig_usr) == SIG_ERR)
		printf("signal SIGSTOP to sig_usr error: %s\n", strerror(errno));
	if (signal(SIGSTOP, SIG_DFL) == SIG_ERR)
		printf("signal SIGSTOP to SIG_DFL error: %s\n", strerror(errno));

	if (signal(SIGUSR1, sig_usr) == SIG_ERR) {
		printf("signal SIGUSR1 error: %s\n", strerror(errno));
		return 1;
	}
	
	if (signal(SIGUSR2, sig_usr) == SIG_ERR) {
		printf("signal SIGUSR1 error: %s\n", strerror(errno));
		return 1;
	}

	for (; ;)
		pause();

	return 0;
}

static void sig_usr(int signum)	/* argument is signal number */
{
	if (signum == SIGUSR1)
		printf("received SIGUSR1\n");
	else if (signum == SIGUSR2)
		printf("received SIGUSR2\n");
}
