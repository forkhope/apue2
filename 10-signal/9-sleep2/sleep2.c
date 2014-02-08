#include <unistd.h>
#include <signal.h>
#include <setjmp.h>

static jmp_buf jmpbuf;

static void sig_alarm(int signo)
{
	longjmp(jmpbuf, 1);
}

/* The SRV2 implementation of sleep() used setjmp() and longjmp() to avoid
 * the race condition described in problem 3 of sleep1() function.
 * The sleep2() function avoids the race condition. Even if the pause() is
 * never executed, the sleep2() function returns when the SIGALRM occurs.
 *
 * There is, however, another subtle problem with the sleep2() function
 * that involves its interaction with other signals. If the SIGALRM
 * interrupts some other signal handler, when we call longjmp(), we abort
 * the other signal handler.
 * SIGALRM信号被递送后,sig_alarm()函数调用longjmp()函数调回到sleep2()函数,
 * 然后sleep2()函数返回,就返回到了调用sleep2()函数的地方,继续往下执行,这里
 * 不会再回到SIGALRM被递送之前所在执行的函数.
 */
unsigned int sleep2(unsigned int seconds)
{
	if (signal(SIGALRM, sig_alarm) == SIG_ERR)
		return seconds;
	if (setjmp(jmpbuf) == 0) {
		alarm(seconds);		/* start the timer */
		pause();			/* next caught signal wakes us up */
	}
	return alarm(0);		/* turn off timer, return unslept time */
}
