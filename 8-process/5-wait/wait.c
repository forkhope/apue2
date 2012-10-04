#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

extern void pr_exit(int status);

/* 关于 wait() 函数的描述,直接看书的 8.6 小节 */
int main(void)
{
	int status;
	pid_t pid;

	if ((pid = fork()) < 0) {
		printf("fork error\n");
		return 1;
	}
	else if (pid == 0) {	/* child */
		exit(7);
	}

	if (wait(&status) != pid) { 	/* wait for pid */
		printf("wait error\n");
		return 1;
	}
	pr_exit(status);		/* and print its status */

	if ((pid = fork()) < 0) {
		printf("fork error\n");
		return 1;
	}
	else if (pid == 0) {	/* child */
		abort();			/* generates ISGABRT */
	}

	if (wait(&status) != pid) { 	/* wait for pid */
		printf("wait error\n");
		return 1;
	}
	pr_exit(status);		/* and print its status */

	if ((pid = fork()) < 0) {
		printf("fork error\n");
		return 1;
	}
	else if (pid == 0) {	/* child */
		status /= 0;		/* divide by 0 generates SIGFPE */
	}

	if (wait(&status) != pid) { 	/* wait for pid */
		printf("wait error\n");
		return 1;
	}
	pr_exit(status);		/* and print its status */

	return 0;
}
