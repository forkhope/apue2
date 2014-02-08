#include "apue.h"
#include <unistd.h>
#include <pthread.h>

void cleanup(void *arg)
{
	printf("cleanup: %s\n", (char *)arg);
}

void *thr_rtn1(void *arg)
{
	int err;
	printf("Thread 1 running ...\n");
	pthread_cleanup_push(cleanup, "thread 1 first handler");
	pthread_cleanup_push(cleanup, "thread 1 second handler");
	if (arg) {
		printf("Thread 1 will cancel thread 3...\n");
		err = pthread_cancel(*((pthread_t *)arg));
		if (err != 0)
			err_quit("can't cancel thread 3: %s\n", strerror(err));
		
		/* 如果一个线程通过 return 返回时, pthread_cleanup_push()注册的
		 * 函数不会被执行.
		 */
		return (void *)1;
	}
	
	/* 由于 pthread_cleanup_push() 和 pthread_cleanup_pop() 可能会用宏
	 * 来实现, pthread_cleanup_push() 中包含 '{', 而 pthread_cleanup_pop()
	 * 中包含 '}', 为了让大括号匹配,这两个函数一定要成对嵌套出现,否则
	 * 编译会报错.正常情况下,下面的代码不会被执行,写在这里是为了能编译通过
	 */
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_exit((void *)1);
}

void *thr_rtn2(void *arg)
{
	printf("Thread 2 running ...\n");
	pthread_cleanup_push(cleanup, "thread 2 first handler");
	pthread_cleanup_push(cleanup, "thread 2 second handler");
	pthread_cleanup_push(cleanup, "thread 2 third handler");
	pthread_cleanup_push(cleanup, "thread 2 four handler");
	
	/* 当 pthread_cleanup_pop() 的参数不是 0 时,它会弹出栈顶的clean-up函数,
	 * 并执行它.
	 */
	pthread_cleanup_pop(1);

	/* 当 pthread_cleanup_pop() 的参数是0时,它也会弹出栈顶的clean-up函数,
	 * 但是不是执行它.所以,执行结果不会打印出"thread 2 third handler"语句.
	 */
	pthread_cleanup_pop(0);
	printf("Thread 2 will terminate ...\n");

	/* 下面通过 pthread_exit() 函数返回, pthreadj_cleanup_push()所注册的
	 * 函数将会被执行,其执行顺序和注册顺序是相反的.
	 */
	if (arg)
		pthread_exit((void *)2);
	pthread_cleanup_pop(0);
	pthread_cleanup_pop(0);
	pthread_exit((void *)2);
}

void *thr_rtn3(void *arg)
{
	printf("Thread 3 running, ID is: %u\n", (unsigned int)pthread_self());
	pthread_cleanup_push(cleanup, "THREAD-3 FIRST HANDLER");
	for (;;) {
		/* 最开始写这个循环时,没有下面的printf()语句,运行程序时,线程3始终
		 * 没有被终止,而加了下面的printf()后,线程3又可以被终止了,当时不知道
		 * 原因.书中12.7小节提到的cancellation point可以解释这个现象.对某个
		 * 线程发出cancel请求后,该请求还是会继续执行,直到遇到cancellation
		 * point才开始真正终止.书中提到printf()可能是(也可能不是)线程取消点,
		 * 所以下面加了printf()后,线程3会被取消.找了一些资料,没有明确的文档
		 * 证明linux下printf()取消点,现在也只是自己根据执行结果得到的推论.
		 */
		printf("Thread 3: waiting ....\n");
	}
	pthread_cleanup_pop(0);
	printf("Exit thread 3.....\n");
	pthread_exit((void *)0);
}

/* One thread can request that another in the same process be canceled by
 * calling the pthread_cancel() function.
 * #include <pthread.h>
 * int pthread_cancel(pthread_t tid);
 * 					Returns: 0 if OK, error number on failure
 * In the default circumstances, pthread_cancel will cause the thread
 * specified by tid to behave as if it had called pthread_exit() with an
 * argument of PTHREAD_CANCELED. However, a thread can select to ignore or
 * otherwise control how it is canceled.
 * Note that pthread_cancel() doesn't wait for the thread to terminate.
 * It merely makes the request.
 *
 * More than one cleanup handler can be established for a thread. The
 * handlers are recorded in a stack, which means that they are executed in
 * the reverse order from that with which they were registered.
 * #include <pthread.h>
 * void pthread_cleanup_push(void (*rtn)(void *), void *arg);
 * void pthread_cleanup_pop(int execute);
 *
 * The pthread_cleanup_push() function schedules the cleanup function, rtn,
 * to be called with the single argument, arg, when the thread performs one
 * of the following actions:
 * 1. Makes a call to pthread_exit().
 * 2. Responds to a cancellation request.
 * 3. Makes a call to pthread_cleanup_pop() with a nonzero execute argument
 * If a thread terminates by returning from its start routine, its cleanup
 * handlers are not called.
 * 关于pthread_cleanup_push()所压入栈的函数什么时候会被执行,可参看man手册.
 *
 * The pthread_cleanup_pop() function removes the routine at the top of the
 * stack of clean-up handlers, and optionally executes it if execute is
 * nonzero.
 *
 * POSIX.1 permits pthread_cleanup_push() and pthread_cleanup_pop() to be
 * implemented as macros that expand to text containing '{' and '}',
 * respectively. For this reason, the caller must ensure that calls to
 * there function are paired within the same function, and at the same
 * lexical nesting level.
 */
int main(void)
{
	pthread_t tid1, tid2, tid3;
	int err;

	err = pthread_create(&tid3, NULL, thr_rtn3, NULL);
	if (err != 0)
		err_quit("create thread 3 error: %s\n", strerror(err));

	err = pthread_create(&tid1, NULL, thr_rtn1, (void *)&tid3);
	if (err != 0)
		err_quit("create thread 1 error: %s\n", strerror(err));

	err = pthread_create(&tid2, NULL, thr_rtn2, (void *)1);
	if (err != 0)
		err_quit("create thread 2 error: %s\n", strerror(err));

	sleep(1);
	pthread_exit((void *)0);
}
