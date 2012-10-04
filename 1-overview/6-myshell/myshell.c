#include "apue.h"
#include <sys/wait.h>

int main(void)
{
	char buf[MAXLINE];
	pid_t pid;
	int status;

	printf("%% ");	/* print prompt (printf() requires %% to print % */
	while (fgets(buf, MAXLINE, stdin) != NULL) {
		if (buf[strlen(buf) - 1] == '\n')
			buf[strlen(buf) - 1] = '\0';  /* replace newline with null */

		if ((pid = fork()) < 0)
			err_sys("myshell: couldn't fork a new process");
		else if (pid == 0) { 	/* child */
			execlp(buf, buf, (char *)0);  /* 最后的0要用char *强制转换 */
			err_ret("myshell: couldn't execute: %s", buf);
			exit(127);
		}

		/* parent */
		if ((pid = waitpid(pid, &status, 0)) < 0)
			err_sys("myshell: couldn't waitpid for pid: %d", pid);
		printf("%% ");
	}

	exit(0);
}
