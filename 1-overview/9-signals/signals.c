#include "apue.h"
#include <sys/wait.h>		/* waipid() */

/* 声明处理 SIGINT 信号的函数 */
static void sig_int(int);	/* our signal-catching function */

int main(void)
{
	char 	buf[MAXLINE];	/* MAXLINE 在 apue.h 中声明 */
	pid_t 	pid;
	int 	status;

	/* 如果调用signal()函数捕获信号失败,该函数返回 SIG_ERR */
	/* To catch a signal, the program needs to call the signal() function,
	 * specifying the name of the function to call when the SIGINT signal
	 * is generated. 在本程序中,捕获到信号后的处理函数是 sig_int().
	 */
	if (signal(SIGINT, sig_int) == SIG_ERR)
		err_sys("signals: signal error");

	printf("%% ");
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0'; /* replace newline with null */

		if ((pid = fork()) < 0)
			err_sys("signals: fork error");
		else if (pid == 0) {			/* child */
			execlp(buf, buf, (char *)0);
			err_ret("signals: couldn't execute: %s", buf);
			exit(127);
		}

		/* parent */
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("signals: waitpid error");
		printf("%% ");
	}
	exit(0);
}

static void sig_int(int signo)
{
	printf("interrupt\n%% ");
}
