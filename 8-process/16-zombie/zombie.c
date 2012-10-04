#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/* Write a program that creates a zombie, and then call system to execute
 * the ps(1) command to verify that the process is a zombie.
 *
 * 书中 8.5 小节提到了僵尸进程(zombie)的概念.
 * The kernel keeps a small amount of information for every terminating
 * process, so that the information is available when the parent of the
 * terminating process calls wait or waitpid. Minimally, this information
 * consists of the process ID, the termination status of the process, and
 * the amount of CPU time taken by the process. The kernel can discard all
 * the memory used by the process and close its open files. In UNIX System
 * terminology, a process that has terminated, but whose parent has not yet
 * waited for it, is called a zombie. The ps(1) command prints the state of
 * a zombie process as Z.
 */
int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0)
		return 1;
	else if (pid == 0) {	/* child */
		/* 一个已经终止,但是其父进程还没有对其进行善后处理(获取终止的子进程
		 * 相关信息,释放它仍占用的资源)的进程被称为僵尸进程.当父进程也退出
		 * 时,该僵尸进程会被init进程自动接手,为它收尸,从而释放该僵尸进程.
		 */
		exit(0);
	}

	/* parent doesn't wait for the child */
	sleep(4);	/* 意图确保子进程已经退出 */

	/* 注意下面的ps命令中,后面的那几个参数之间不能有空格,
	 * 即写为pid, ppid时, ps 命令会报错.
	 */
	system("ps -o pid,ppid,state,tty,command");

	return 0;
}
