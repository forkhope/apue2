#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

/* If we want to write a process so that it forks a child but we don't want
 * to wait for the child to complete and we don't want the child to become
 * a zombie until we terminate, the trick is to call fork() twice.
 */
int main(void)
{
	pid_t pid;

	if ((pid = fork()) < 0) {
		printf("fork error\n");
		return 1;
	}
	else if (pid == 0) {	/* first child */
		if ((pid = fork()) < 0) {
			printf("fork again error\n");
			return 1;
		}
		else if (pid > 0)
			exit(0);	/* parent from second fork == first child */

		/* We're the second child; our parent becomes init as soon as our
		 * real parent calls exit() in the statement above. Here's where
		 * we'd continue executing, knowing that when we're done, init
		 * will reap our status.
		 *
		 * We call sleep() in the second child to ensure that the first
		 * child terminates before printing the parent process ID. After
		 * a fork(), either the parent or the child can continue executing;
		 * we never know which will resume execution first. If we don't put
		 * the second child to sleep, and if it resumed execution after the
		 * fork() before its parent, the parent process ID that it printed
		 * would be that of its parent, not process ID 1.
		 */
		sleep(2);
		printf("second child pid = %d, parent pid = %d\n", 
				getpid(), getppid());

		/* 如果没有下面这句 exit(0), 则执行会打印出"wait error",也就是打印
		 * 下面wait()函数执行出错时的出错信息.但是要注意的是,此时并不是父进
		 * 程执行waitpid函数出错,而是第二个子进程执行waitpid函数出错,因为
		 * 第二个子进程并没有属于它的子进程(打印结果中带有进程号,对比即知).
		 * 在这段代码中,看起来第二个子进程的代码被上面的 else if 分支包含起
		 * 来,但是并这不表示第二个子进程的代码只在这个 else if 分支中,代码
		 * 执行到这个 else if 分支的最后一个右花括号时,第二个子进程也不会
		 * 自动退出,而是会继续往下执行,当执行到下面的waitpid()时,由于第二个
		 * 子进程并没有属于它自己的子进程,waitpid()函数报错返回,程序中打印
		 * 出错信息.所以,如果想让第二个子进程的代码只在else if分支内,需要
		 * 调用下面的exit()函数,或者return语句,或者使用其他的方法结束进程.
		 */
		exit(0);
	}

	if (waitpid(pid, NULL, 0) != pid)	/* wait for first child */
		printf("wait error, pid = %d\n", getpid());
	else
		printf("wait success, pid = %d\n", getpid());

	/* We're the parent (the original process); we continue executing,
	 * knowing that we're not the parent of the second child.
	 */
	return 0;
}
