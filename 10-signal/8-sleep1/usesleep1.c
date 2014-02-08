#include <stdio.h>
#include <unistd.h>
#include <signal.h>

extern unsigned int sleep1(unsigned int seconds);
static void my_alarm(int signo);

/* use the sleep1() function. */
int main(void)
{
	unsigned int unslept;

	if (signal(SIGALRM, my_alarm) == SIG_ERR) {
		printf("signal SIGALAR error: %m\n");
		return 1;
	}
	alarm(10);

	/* 调用 sleep1() 函数时, 上面的 alarm(10) 警报定时器将会被取消,同时也
	 * 改变了上面所设置的 SIGALRM 信号处理函数,且不会恢复,即调用 sleep1()
	 * 函数结束后, SIGALRM 信号的处理函数也不会被恢复成 my_alarm().
	 */
	unslept = sleep1(2);
	printf("The return value of sleep(2) is: %u\n", unslept);

	/* 下面返回的 i 将是 0, 因为sleep1() 函数最后返回之前调用了 alarm(0)
	 * 函数取消了警报定时器,即此时没有警报定时器的剩余时间,所以返回为 0.
	 */
	unslept = alarm(1);
	printf("The return value of alarm(1) is: %u\n", unslept);

	/* 上面的 alarm(1) 语句所设置的警报定时器超时后,不会调用 my_alarm()
	 * 函数,调用sleep1()函数会改变SIGALRM信号的处理函数,且不会自动恢复.
	 * sleep1()函数的SIGALRM信号处理函数会打印一个字符串,这里打印的正是
	 * 这个字符串,而不是下面的 "main(), received SIGALRM\n"
	 */
	pause();
	return 0;
}

static void my_alarm(int signo)
{
	printf("main(), received SIGALRM\n");
}
