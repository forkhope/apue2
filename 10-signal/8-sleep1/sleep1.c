#include <signal.h>
#include <stdio.h>
#include <unistd.h>

static void sig_alarm(int signo)
{
	printf("The signal handler of SIGALRM in the sleep1() function\n");
}

/* simply, incomplete sleep() function
 * Using alarm() and pause(), we can put a process to sleep for a specified
 * amount of time. 注意,当有信号产生时,sleep()函数会被提前唤醒.
 * 下面的这个简易版的sleep()函数有三个问题:
 * 1. If the caller already has an alarm set, that alarm is erased by the
 * first call to alarm(). We can correct this by looking at the return value
 * from the first call to alarm(). If the number of seconds until some
 * previously set alarm is less than the argument, then we should wait only
 * until the previously set alarm expires. (此时,sleep1()函数被提前唤醒,如
 * 果之前设置过alarm(3),然后再设置sleep1(6),按照上面的描述,sleep1()函数会放
 * 任alarm(3)超时,发送信号SIGALRM将sleep1()函数唤醒.) If the previously set
 * alarm will go off after ours, then before returning we should reset this
 * alarm to occur at its designated time in the future.
 * 2. We have modified the disposition for SIGALRM. If we're writing a
 * function for others to call, we should save the disposition when we're
 * called and restore it when we're done. We can correct this by saving
 * the return value from signal() and resetting the disposition before we
 * return.
 * 3. There is a race condition between the first call to alarm() and the
 * call to pause(). On a busy system, it's possible for the alarm to go off
 * and the signal handler to be called before we call pause(). There are two
 * ways to correct this. The first uses setjmp(). The other uses
 * sigprocmask() and sigsuspend().
 *
 * man 3 sleep手册中,对 sleep() 函数的解释如下:
 * sleep() makes the calling thread sleep until seconds(指sleep()函数传入的
 * 秒数) seconds have elapsed or a signal arrives which is not ignored.
 * The RETURN VALUE of sleep() is:
 * Zero if the requested time has elapsed, or the number of seconds left to
 * sleep, if the call was interrupted by a signal handler.
 *
 * 下面的代码和这两段描述是能对应上的.
 * 下面先调用alarm(seconds)函数设置一个seconds秒的警报定时器,然后再调用pause
 * 函数陷入休眠,等待信号过来,如果在seconds秒内,有其他信号产生,pause()会被唤
 * 醒,然后sleep()函数再调用alarm(0)取消还没有超时的警报定时器,并返回这个语句
 * 的返回值,即返回上一个警报定时器的剩余时间. 如果在seconds秒内,没有其他信号
 * 产生,则会一直等到seconds秒超时后,alarm()函数产生SIGALRM信号,唤醒pause(),
 * 此时alarm(0)会返回0.
 */
unsigned int sleep1(unsigned int seconds)
{
	if (signal(SIGALRM, sig_alarm) == SIG_ERR)
		return seconds;
	alarm(seconds);			/* start the timer */
	pause();				/* next caught signal wakes us up*/
	return (alarm(0));		/* turn off timer, return unslept time */
}
