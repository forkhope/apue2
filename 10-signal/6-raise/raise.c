#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

static void sig_usr1(int signo)
{
	printf("received SIGUSR1\n");
}

/* The raise function allows a process to send a signal to itself.
 * #include <signal.h>
 * int raise(int signo);
 * 		Returns: 0 if OK, nonzero if failure.
 * APUE2书中 10.9 小节描述 raise() 在失败时返回 -1,这个查阅man手册和POSIX.1
 * 的相关资料,发现书中描述应该是错的, raise() 失败时,返回一个非 0 的值.
 *
 * raise() was originally defined by ISO C.
 * The call 
 * 		raise(signo); 
 * is equivalent to the call
 * 		kill(getpid(), signo);
 */
int main(void)
{
	if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
		printf("signal SIGUSR1 error: %s\n", strerror(errno));
		return 1;
	}

	if (raise(SIGUSR1) != 0) {
		printf("raise error: %s\n", strerror(errno));
		return 1;
	}

	/* raise() 函数应该是和 kill() 函数类似,将会在递送信号之后再返回. */
	printf("After raise(), ......\n");
	return 0;
}
