#include <stdio.h>
#include <signal.h>

/* We need a data type to represent multiple signals--a signal set. We'll
 * use this with such function as sigprocmask() to tell the kernel not to
 * allow any of the signals in the set to occur.
 * 由于系统中信号的数目可能超过int型的位数,所以 POSIX.1 defines the data
 * type sigset_t to contain a signal set and the following five functions
 * to manipulate signal sets.
 * #include <signal.h>
 * int sigemptyset(sigset_t *set);
 * int sigfillset(sigset_t *set);
 * int sigaddset(sigset_t *set, int signo);
 * int sigdelset(sigset_t *set, int signo);
 * 		All four return: 0 if OK, -1 on error
 * int sigismember(const sigset_t *set, int signo);
 * 		Returns: 1 if true, 0 if false, -1 on error
 *
 * The function sigemptyset() initializes the signal set pointed to by set
 * so that all signals are excluded. The function sigfillset() initializes
 * the signal set so that all signals are included. All applications have
 * to call either sigemptyset() or sigfillset() once for each signal set,
 * before using the signal set
 */
int main(void)
{
	sigset_t set;

	if (sigemptyset(&set) < 0) {
		printf("sigemptyset error: %m\n");
		return 1;
	}

	printf("after sigemptyset: whether SIGALRM in the set: %s\n", 
			sigismember(&set, SIGALRM) == 1 ? "TRUE" : "FALSE");

	if (sigaddset(&set, SIGALRM) < 0) {
		printf("sigaddset error: %m\n");
		return 1;
	}

	printf("After sigaddset: whether SIGALRM in the set: %s\n", 
			sigismember(&set, SIGALRM) == 1 ? "TRUE" : "FALSE");

	/* 如果某个信号已经存在于信号集set中,再次添加该信号到set中,不会报错 */
	if (sigaddset(&set, SIGALRM) < 0) {
		printf("sigaddset error: %m\n");
		return 1;
	}

	if (sigdelset(&set, SIGALRM) < 0) {
		printf("sigdelset error: %m\n");
		return 1;
	}

	printf("After sigaddset: whether SIGALRM in the set: %s\n", 
			sigismember(&set, SIGALRM) == 1 ? "TRUE" : "FALSE");

	return 0;
}
