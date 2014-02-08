#include <stdio.h>
#include <unistd.h>
#include <signal.h>

volatile sig_atomic_t quitflag;	/* set nonzero by signal handler */

/* one signal handler for SIGINT and SIGQUIT */
static void sig_handler(int signum)
{
	if (signum == SIGINT)
		printf("SIGINT: interrupt\n");
	else if (signum == SIGQUIT)
		quitflag = 1;		/* set flag for main loop */
}

/* Another use of sigsuspend() is to wait for a signal handler to set a
 * global variable. In this program, we catch both the interrupt signal and
 * the quit signal, but want to wake up the main routine only when the
 * quit signal is caught.
 */
int main(void)
{
	struct sigaction act;
	sigset_t newmask, oldmask, zeromask;

	act.sa_handler = sig_handler;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_RESTART;
	if (sigaction(SIGQUIT, &act, NULL) < 0)
		return 1;
	if (sigaction(SIGINT, &act, NULL) < 0)
		return 1;

	sigemptyset(&zeromask);
	sigemptyset(&newmask);
	sigaddset(&newmask, SIGQUIT);

	/* Block SIGQUIT and save current signal mask. */
	if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0)
		return 1;

	while (quitflag == 0)
		sigsuspend(&zeromask);

	/* SIGQUIT has been caught and is now blocked; do whatever. */
	quitflag = 0;

	/* Reset signal mask which unblocks SIGQUIT. */
	if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0)
		return 1;

	return 0;
}
