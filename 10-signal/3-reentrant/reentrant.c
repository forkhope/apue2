#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>

/* 执行这个函数时,它会调用 getpwnam() 函数,在这个程序中,这样做可能会
 * 导致死锁,在下面对此有描述.
 */
static void my_alarm(int signum)
{
	struct passwd *rootptr;

	/* 实际执行时,可以输出这一句 */
	printf("in SIGALRM signal handler\n");
	if ((rootptr = getpwnam("root")) == NULL) {
		printf("getpwnam(root) error: %m");
		exit(1);
	}

	/* 运行的时候,可能看不到这一句输出,因为上面的getpwnam()函数可能会阻塞.
	 * 在实际执行的时候，这一句总是看不到,getpwnam()每次都会阻塞.
	 */
	printf("getpwnam(root): pw_name: %s\n", rootptr->pw_name);
	alarm(1);
}

/* When a signal that is being caught is handled by a process, the normal
 * sequence of instructions being executed by the process is temporarily
 * interrupted by the signal handler. The process then continues executing,
 * but the instructions in the signal handler are now executed. If the
 * signal returns (instead of calling exit or longjmp, for example), then
 * the normal sequence of instructions that the process was executing when
 * the signal was caught continues executing.
 * 根据书中10.6小节的描述,有些函数如果同时在进程和信号处理函数中调用时,可能
 * 会带来混乱.下面几种类型的函数就不应该在信号处理函数中被调用:
 * (a)they are known to use static data structures,
 * (b)they call malloc() or free(),
 * (c)they are part of the standard I/O library. Most implementations of the
 *    standard I/O library use global data structures in a nonreentrant way.
 *    Note that even though we call printf() from signal handlers in some of
 *    our example, it is not guaranteed to produce the expected results,
 *    since the signal hander can interrupt a call to printf() from out main
 *    program. 即,printf()也是不可重入函数,最好不要在信号处理函数中调用它.
 *
 * 书中给出了一个可重入函数的列表. Be aware that even if we call a function
 * listed in Figure 10.4 from a signal handler, there is only one errno
 * variable per thread, and we might modify its value. Therefore, as a
 * general rule, when calling the functions listed in Figure 10.4 from a
 * signal handler, we should save and restore errno.
 */
int main(void)
{
	struct passwd *ptr;

	signal(SIGALRM, my_alarm);
	alarm(1);

	for (; ;) {
		/* 查看glibc源码pwd/getpwnam.c文件,发现getpwnam()函数的具体实现是
		 * 在nss/getXXbyYY.c中的,这个函数在开始执行的时候,会先调用
		 * __libc_lock_lock()函数,这个函数调用pthread_mutex_lock()来加锁.
		 *
		 * 当下面的getpwnam()函数在加锁之后,解锁之前,程序接到了上面alarm()
		 * 函数发来的SIGALRM信号,则程序转去执行信号处理函数my_alarm(),而
		 * 这个函数也会调用getpwnam()函数,然后试图去加锁,导致死锁.因为在
		 * 下面调用getpwnam("lixianyi")时申请的锁还没有解开,信号处理函数中的
		 * getpwnam("root")获取不到锁,会阻塞住;但是getpwnam("lixianyi")需要
		 * 得到执行机会才能把锁解开,而信号处理函数没有执行结束之前,是不会
		 * 重新执行到getpwnam("lixianyi")这里来的,双方相互等待,陷入死锁.
		 *
		 * 从中可以看到,需要尽量避免在信号处理函数中调用不可重入函数,以免
		 * 发生死锁.即使是在信号处理函数中调用可重入函数列表中的函数,也要
		 * 注意先保存errno的值. 
		 */
		if ((ptr = getpwnam("lixianyi")) == NULL) {
			printf("getpwnam error: %m");
			exit(1);
		}
		if (strcmp(ptr->pw_name, "lixianyi") != 0)
			printf("return value corrupted! pw_name = %s\n", ptr->pw_name);
	}
	return 0;
}
