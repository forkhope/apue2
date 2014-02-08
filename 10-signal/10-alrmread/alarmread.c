#include <stdio.h>
#include <unistd.h>
#include <signal.h>

static void sig_alarm(int signum)
{
	printf("received SIGALRM\n");
}

/* A common use for alarm(), in addition to implementing the sleep()
 * function, is to put an upper time limit to operations that can block.
 * For example, if we have a read operation on a device that can block, we
 * might want the read() to time out after some amount of time.
 * 
 * This sequence of code is common in UNIX applications, but this program
 * has two problems.
 * 1. The program has one of the same flaws that we described in sleep1(),
 * a race condition between the first call to alarm() and the call to read()
 * 2. If system calls are automatically restarted, the read is not
 * interrupted when the SIGALRM signal handler returns. In this case, the
 * timeout does nothing.
 * Linux (2.6.32-5-686) 中, read()函数默认好像就是中断自动重入,所以运行这个
 * 程序时,read()函数没有被打断,而是会一直阻塞,为了确认SIGALRM信号确实产生过
 * 在sig_alarm()函数中打印了一个字符串,运行时,能看到这个字符串的输出.
 *
 * 我们可以使用longjmp()函数来避免第一个问题,但是使用longjmp()函数可能会
 * 中断其他信号处理函数;就是说,各有各的缺陷.
 */
int main(void)
{
	int n;
	char buf[BUFSIZ];

	if (signal(SIGALRM, sig_alarm) == SIG_ERR)
		return 1;

	alarm(5);
	if ((n = read(STDIN_FILENO, buf, BUFSIZ)) < 0)
		return 1;
	alarm(0);		/* 取消定时器 */

	write(STDOUT_FILENO, buf, n);
	return 0;
}
