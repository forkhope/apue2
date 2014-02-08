#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

static void sig_int(int signum)
{
	printf("received SIGINT\n");
}

static void sig_usr1(int signum)
{
	printf("received SIGUSR1\n");
}

/* The kill() function sends a signal to a process or a group of processes.
 * #include <signal.h>
 * int kill(pid_t pid, int signo);
 * 		Returns: 0 if OK, -1 on error.
 *
 * There are four different conditions for the pid argument to kill().
 * pid >  0: The signal is sent to the process whose process ID is pid.
 * pid == 0: The signal is sent to the processes whose process group ID
 * 			 equals the process group ID of the sender and for which the
 * 			 sender has permission to send the signal. Note that the term
 * 			 all processes excludes an implementation-defined set of system
 * 			 processes. For most UNIX systems, this set of system processes
 * 			 includes the kernel processes and init (pid 1).
 * pid <  0: The signal is sent to all processes whose process group ID
 * 			 equals the absolute value of pid and for which the sender has
 * 			 permission to send the signal. Again, the set of all processes
 * 			 excludes certain system processes, as described earlier.
 * pid ==-1: The signal is sent to all process on the system for which the
 * 			 sender has permission to send the signal. As before, the set
 * 			 of processes excludes certain system processes.
 *
 * As we've mentioned, a process needs permission to send a signal to
 * another process. The superuser can send a signal to any process. For
 * other users, the basic rule is that the real or effective user ID of the
 * sender has to equal the real or effective user ID of the receiver. If the
 * implementation supports _POSIX_SAVED_IDS, the saved set-user-ID of the
 * receiver is checked instead of its effective user ID. There is also one
 * special case for the permission testing: if the signal being sent is
 * SIGCONT, a process can send it to any other process in the same sessioin.
 *
 * POSIX.1 defines signal number 0 as the null signal. If the signo argument
 * is 0, then the normal error checking is performed by kill(), but no
 * signal is sent. This is often used to determine if a specific process
 * still exists. If we send to the process the null signal and it doesn't
 * exist, kill() returns -1 and errno is set to ESRCH. Be aware, however,
 * that UNIX systems recycle process IDs after some amount of time, so the
 * existence of a process with a given process ID does not mean that it's
 * the process that you think it is.
 * Also understand that the test for process existence is not atomic. By
 * the time that kill() returns the answer to the caller, the process in
 * question might have existed, so the answer is of limited value.
 *
 * If the call kill() causes the signal to be generated for the calling
 * process and if the signal is not blocked, either signo or some other
 * pending, unblocked signal is delivered to the process before kill()
 * returns.
 */
int main(void)
{
	pid_t pid;
	int i;

	if ((pid = fork()) < 0)
		return 1;
	else if (pid == 0) {	/* child */
		if (signal(SIGUSR1, sig_usr1) == SIG_ERR) {
			printf("signal SIGUSR1 error: %m\n");
			return 1;
		}
		pause();

		printf("child will be terminated\n");
		exit(0);
	}

	/* 这里休眠 5秒,意图让子进程先得到执行机会,否则运行时会得不到想要的结果
	 * 由于fork()之后,父进程和子进程之间的执行顺序不定,如果父进程先得到执行
	 * 机会,且没有下面的sleep(5)语句,则由于子进程还没有执行,它也就还没有将
	 * SIGUSR1的处理函数设为sig_usr1(),所以此时子进程对SIGUSR1的处理行为是
	 * 默认处理,即终止进程.所以父进程在下面语句中调用kill()函数给子进程发送
	 * SIGUSR1信号后,子进程会终止,而此时父进程还没有清理子进程的退出状态,子
	 * 进程会成为一个僵尸进程.
	 *
	 * 一开始写这个程序时,没有加下面sleep(5)语句,程序的执行结果让我很困惑.
	 * 使用下面的kill()函数给子进程发送SIGUSR1后,为什么看不到sig_usr1()函数
	 * 打印出的"received SIGUSR1"呢?分析之后,我觉得原因就是上面所描述的那样
	 * 简单说就是,父进程先于子进程执行,子进程还没有来得及改变SIGUSR1的处理
	 * 函数为sig_usr1(),所以子进程在这个时候接收到SIGUSR1信号时,不会调用
	 * sig_usr1()函数,而是让SIGUSR1的默认行为发生(即终止子进程),也就看不到
	 * sig_usr1()函数里面的"received SIGUSR1"打印出来了.
	 */
	sleep(5);

	if (kill(pid, SIGUSR1) < 0) {
		printf("kill(%d, SIGUSR1) error\n", pid);
		return 1;
	}

	/* 上面提到,如果没有sleep(5)语句,且父进程先于子进程执行,则子进程会在接
	 * 收到SIGUSR1后终止,此时父进程还没有为子进程"收尸",子进程成为僵尸进程
	 * 查看下面的ps命令输出结果可以看到,子进程确实是僵尸进程(在ps命令中,
	 * 僵尸进程的状态用大写字母Z表示,下面的state参数会打印出进程的状态).
	 */
	system("ps -o pid,ppid,state,tty,command");

	/* 我一开始在写这个程序时,认为一个进程调用终止后(如调用exit()函数退出),
	 * 使用这个进程的pid作为参数执行kill(pid, 0)语句,kill()函数应该会返回-1
	 * 并将errno设为ESRCH(假设这个pid还没有分配给新的进程).但实际上不一定.
	 * 对一个僵尸进程来说,使用它的pid来执行kill(pid, 0)语句,kill()函数还是
	 * 会返回0.虽然此时这个僵尸进程没有在运行,但是它还存在于进程表中,kill()
	 * 函数并没有认为僵尸进程是一个不存在的进程.
	 * 例如在这个程序中,当子进程成为僵尸进程后,下面的 i 的输出结果是 0.
	 *
	 * 也就是说,进程在终止之后,它的父进程为它"收尸"之前,它会依然存在于进程
	 * 表中.在不能观念上认为进程终止后,它就立刻不存在了.
	 *
	 * man 2 kill手册中,也提到了僵尸进程是一个依然存在的进程,如下:
	 * ESRCH  The pid or process group does not exist. Note that an existing
	 *        process might be a zombie, a process which  already committed
	 *        termination, but has not yet been wait(2)ed for.
	 */
	i = kill(pid, 0);
	printf("The return value of kill(%d, 0) is: %d\n", pid, i);

	/* 给一个僵尸进程发信号是允许的,kill()函数不会报错.因为僵尸进程是一个
	 * 依然存在的进程,如上面所述.
	 */
	i = kill(pid, SIGUSR1);
	printf("The return value of kill(%d, SIGUSR1) is: %d\n", pid, i);

	/* 父进程为子进程"收尸" */
	if (waitpid(pid, NULL, 0) < 0) {
		printf("waitpid error\n");
		return 1;
	}

	/* 父进程为子进程"收尸"后,子进程就真的不存在了,此时对子进程调用kill()
	 * 函数,将会报错,且 errno 被置为 ESRCH.
	 */
	i = kill(pid, 0);
	printf("After wait for pid %d, kill(%d, 0) returned: %d",pid,pid,i);
	if (errno == ESRCH)
		printf(", and errno == ESRCH\n");

	/* 为父进程安装 sig_int() 函数到 SIGINT 信号上 */
	if (signal(SIGINT, sig_int) == SIG_ERR) {
		printf("signal SIGINT error: %m\n");
		return 1;
	}

	/* 父进程发送 SIGINT 信号给自身.
	 * 上面提到,在 kill() 函数返回之前,已经有信号被递送.即会先执行信号的
	 * 处理函数,kill()函数才会返回.所以下面会先打印sig_int()函数中的
	 * "received SIGINT",再打印后面的"The log will ..."(省略部分字符).
	 */
	if (kill(getpid(), SIGINT) < 0) {
		printf("kill itself error\n");
		return 1;
	}
	printf("This log will be printed after the sig_int() returns\n");

	return 0;
}
