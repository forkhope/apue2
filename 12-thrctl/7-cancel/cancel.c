#include <stdio.h>
#include <string.h>
#include <pthread.h>

void *thr_rtn(void *arg)
{
	int state, type, err;

	printf("Enter thread 1...\n");
	printf("Set the tread state to PTHREAD_CANCEL_DISABLE\n");
	err = pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &state);
	if (err != 0) {
		printf("pthread_setcancelstate error: %s\n", strerror(err));
		return (void *)1;
	}
	printf("The old cancel state is: %d, ", state);
	if (state == PTHREAD_CANCEL_DISABLE)
		printf("PTHREAD_CANCEL_DISABLE\n");
	else if (state == PTHREAD_CANCEL_ENABLE)
		printf("PTHREAD_CANCEL_ENABLE\n");
	else
		printf("**UNKNOWN**\n");

	printf("Set the tread type to PTHREAD_CANCEL_ASYNCHRONOUS\n");
	err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &type);
	if (err != 0) {
		printf("pthread_setcanceltype error: %s\n", strerror(err));
		return (void *)1;
	}
	printf("The old cancel type is: %d, ", type);
	if (type == PTHREAD_CANCEL_DEFERRED)
		printf("PTHREAD_CANCEL_DEFERRED\n");
	else if (type == PTHREAD_CANCEL_ASYNCHRONOUS)
		printf("PTHREAD_CANCEL_ASYNCHRONOUS\n");
	else
		printf("**UNKNOWN**\n");
	
	return (void *)0;
}

void *thr_rtn2(void *arg)
{
	int err, type;

	printf("Enter thread 2.....\n");
	err = pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &type);
	if (err != 0) {
		printf("Thread 2, pthread_setcanceltype error: %s\n",strerror(err));
		pthread_exit((void *)1);
	}

	printf("Thread 2, enter for ..... \n");
	for (;;) {
	}

	/* 线程被取消之后,会从取消点那里直接退出,而不是继续往下执行,所以线程3
	 * 会从上面的for循环退出,不会执行到下面这里来,下面的printf语句不会被打印
	 */
	printf("Exit thread 2 .......\n");
	pthread_exit((void *)0);
}

/* Two thread attributes that are not included in the pthread_attr_t
 * structure are the cancelability state and the cancelability type. These
 * attributes affect the behavior of a thread in response to a call to
 * pthread_cancel().
 * The cancelability state attribute can be either PTHREAD_CANCEL_ENABLE or
 * PTHREAD_CANCEL_DISABLE. A thread can change its cancelability state by
 * calling pthread_setcancelstate().
 * #include <pthread.h>
 * int pthread_setcancelstate(int state, int *oldstate);
 * 				Returns: 0 if OK, error number on failure
 * In one atomic operation, pthread_setcancelstate() sets the current
 * cancelability state to state and stores the previous cancelability state
 * in the memory location pointed to by oldstate.
 *
 * Recall from Section 11.5 that a call to pthread_cancel() doesn't wait for
 * a thread to terminate. In the default case, a thread will continue to
 * execute after a calcellation request is made, until the thread reaches a
 * cancellation point. A cancellation point is a place where the thread
 * checks to see whether it has been chanceled, and then acts on the request
 * 书中列举了取消点相应的函数和可能是取消点的相应函数.
 * A thread starts with a default cancelability state of
 * PTHREAD_CANCEL_ENABLE. When the state is set to PTHREAD_CANCEL_DISABLE,
 * a call to pthread_cancel() will not kill the thread. Instread, the
 * cancellation request remains pending for the thread. When the state is
 * enabled again, the thread will act on any pending cancellation requests
 * at the next cancellation point.
 *
 * If your application doesn't call one of the functions in the list for a
 * long period of time, then you can call pthread_testcancel() to add your
 * own cancellation points to the program.
 * void pthread_testcancel(void);
 * When you call pthread_testcancel, if a cancellation request is pending
 * and if cancellation has not been disabled, the thread will be canceled.
 * When cancellation is disabled, however, calls to pthread_testcancel have
 * no effect.
 *
 * The default cancellation type we have been describing is known as
 * deferred cancellation. After a call to pthread_cancel(), the actual
 * cancellation doesn't occur until the thread hits a cancellation point.
 * We can change the cancellation type by calling pthread_setcanceltype().
 * int pthread_setcanceltype(int type, int *oldtype);
 * 				Returns: 0 if 0K, error number on failure
 * The type parameter can be either PTHREAD_CANCEL_DEFERRED or
 * PTHREAD_CANCEL_ASYNCHRONOUS. The pthread_setcanceltype() function sets
 * the cancellation type to type and returns the previous type in the
 * integer pointed to by oldtype.
 * Asynchronous cancellation differs from deferred cancellation in that the
 * thread can be canceled at any time. The thread doesn't necessarily need
 * to hit a cancellation point for it to be canceled.
 *
 * 关于上面两个函数的第二个参数是否可以设为NULL, man手册的说明如下:
 * The Linux threading implementations premit the oldstate argument of
 * pthread_setcancelstate() to be NULL, in which case the information about
 * the previous cancelability state is not returned to the caller. Many
 * other implementations also permit a NULL oldstate argument, but POSIX.1
 * does not specify this point, so portable applications should always
 * specify a non-NULL value in oldstate. A precisely analogous set of
 * statements applies for the oldtype argument of pthread_setcanceltype().
 */
int main(void)
{
	pthread_t tid1, tid2;
	int err;

	err = pthread_create(&tid1, NULL, thr_rtn, NULL);
	if (err != 0)
		return 1;

	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0)
		return 1;

	sleep(1);
	printf("main thread: cancel thread 2\n");
	pthread_cancel(tid2);

	pthread_exit((void *)0);
}
