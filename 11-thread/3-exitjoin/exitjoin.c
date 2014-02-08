#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include <pthread.h>

void err_quit(const char *, ...);

/* 书中是用硬编码的方式写了两个函数,而下面是写成了一个函数,程序运行结果为:
 * 		Thread 2: enter ...
 * 		Thread 2: enter ...
 * 		thread 1 exit, code: 2
 * 		thread 2 exit, code: 2
 * 在main()函数中,先执行arg=1; thr_rtn(arg),再执行arg=2; thr_rtn(arg),本该
 * 是打印 "Thread 1: enter ..." 和 "Thread 2: enter ...",但是结果却是打印了
 * 两个 "Thread 2: enter ...",和预期有所偏差.经过分析,原因应该是:
 * 创建线程后,原先线程和被创建线程的执行顺序是不确定的,当原先程序先执行时,
 * main()函数中就会将arg置为2; 之后,线程1得到执行机会,它通过*arg来进行指针
 * 解引用,得到的arg值就变成了2,导致打印出来两个 "Thread 2: enter ...".
 * 由此可见,多线程编程一定要警惕对数据的共享访问,避免可能的数据重写覆盖.
 */
void *thr_rtn(void *arg)
{
	printf("Thread %d: enter ...\n", *((int *)arg));
	pthread_exit(arg);
}

/* If any thread within a process calls exit, _Exit, or _exit, then the
 * entire process terminates.
 * A single thread can exit in three ways, thereby stopping its flow of
 * control, without terminating the entire process.
 * 1. The thread can simply return from the start routine. The return value
 *    is the thread's exit code.
 * 2. The thread can be canceled by another thread in the same process.
 * 3. The thread can call pthread_exit().
 *
 * #include <pthread.h> ---- void pthread_exit(void *rval_ptr);
 * The pthread_exit() function terminates the calling thread and returns a
 * value via rval_ptr that (if the thread is joinable) is available to
 * another thread in the same process that calls pthread_join().
 * Any clean-up handlers established by pthread_cleanup_push() that have
 * not yet been popped, are popped (in the reverse of the order in which
 * they were pushed) and executed.
 * The value pointed to by rval_ptr should not be located on the calling
 * thread's stack, since the contents of that stack are undefined after
 * the thread terminates.
 *
 * #include <pthread.h>
 * int pthread_join(pthread_t thread, void **rval_ptr);
 * 					Returns: 0 if OK, error number on failure
 * The pthread_join() function waits for the thread specified by thread to
 * terminate. If that thread has already terminated, then pthread_join()
 * returns immediately. The thread specified by thread must be joinable.
 * If retval is not NULL, then pthread_join() copies the exit status of the
 * target thread (i.e., the value that the target thread supplied to
 * pthread_eixt()) into the location pointed to by *retval. If the target
 * thread was canceled, then PTHREAD_CANCELED is placed in *retval. 
 * If the thread calling pthread_join() is canceled, then the target thread
 * willremain joinable (i.e., it will not be detached). Joining with a 
 * thread that has previously been joined results in undefined behabvior.If
 * we're not interested in a thread's return value, we can set retval to 
 * NULL. By calling pthread_join(), we automatically place a thread in the
 * detached state so that its resources can be recovered. If the thread was
 * already in the detached state, calling pthread_join() fails, returning
 * EINVAL.
 */
int main(void)
{
	int err, arg;
	pthread_t tid1, tid2;
	void *rval;

	arg = 1;
	err = pthread_create(&tid1, NULL, thr_rtn, (void *)&arg);
	if (err != 0)
		err_quit("create thread 1 error: %s", strerror(err));

	arg = 2;	/* 这里对arg赋值为2,导致程序运行可能出错,如上所述 */
	err = pthread_create(&tid2, NULL, thr_rtn, (void *)&arg);
	if (err != 0)
		err_quit("create thread 2 error: %s", strerror(err));

	/* 主线程调用pthread_join()函数后,将会被阻塞,直到tid1对应的线程执行结束
	 * 为止.从执行结果上看,会先打印线程1里面的语句,再打印下面的语句.
	 */
	err = pthread_join(tid1, &rval);
	if (err != 0)
		err_quit("join thread 1 error: %s", strerror(err));
	printf("thread 1 exit, code: %d\n", *((int *)rval));

	err = pthread_join(tid2, &rval);
	if (err != 0)
		err_quit("join thread 2 error: %s", strerror(err));
	printf("thread 2 exit, code: %d\n", *((int *)rval));

	/* 如果上面没有调用pthread_join()等待新创建的线程执行结束,则当主线程
	 * 执行下面的exit(0)后,整个进程都会终止,新创建的线程可能都得不到执行
	 * 机会. 所以 man pthread_exit 的 NOTES 小节中提到:
	 * To allow other threads to continue execution, the main thread should
	 * terminate by calling pthread_exit() rather than exit().
	 */
	exit(0);
}

void err_quit(const char *fmt, ...)
{
	char buf[1024];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, 1024, fmt, ap);
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}
