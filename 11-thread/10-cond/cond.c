#include <stdio.h>
#include <sys/time.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int state = 0;

/* 获取当前时间并得到pthread_cond_timedwait()要等待的绝对时间 */
void maketimeout(struct timespec *tsp, long long msec)
{
	struct timeval tv;

	gettimeofday(&tv, NULL);
	tsp->tv_sec = tv.tv_sec + msec;
	tsp->tv_nsec = tv.tv_usec * 1000;
}

void *thr_rtn1(void *arg)
{
	int i;

	sleep(2);
	state = 1;
	/* 下面调用pthread_cond_signal()函数一次至少唤醒一个等待的线程,测试
	 * 发现,如果将下面的 i < 2 写为 i < 1,主线程和线程2只会被唤醒一个.如果
	 * 是主线程先被唤醒,线程2将不会被唤醒;如果是线程2先唤醒,则主线程会在超
	 * 时后被唤醒.至于,主线程和线程2哪个会先被唤醒是不确定的,并不是先进入
	 * 阻塞的线程就会被先唤醒,而是取决于操作系统的调度.
	 */
	for (i = 0; i < 2; ++i) {
		pthread_mutex_lock(&mutex);
		printf("thread 1: i: %d\n", i);
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
	}
	pthread_exit((void *)0);
}

void *thr_rtn2(void *arg)
{
	pthread_mutex_lock(&mutex);
	while (state == 0) {
		printf("thread 2, while (state == 0)\n");
		pthread_cond_wait(&cond, &mutex);
	}

	/* 从执行结果看,可能会先打印"thread 2, wakeup",也可能会先打印"main
	 * thread, wakeup",说明主线程和线程2的唤醒顺序确实是不固定的.
	 */
	printf("thread 2, wakeup.........\n");
	pthread_mutex_unlock(&mutex);
	pthread_exit((void *)0);
}

/* When used with mutexes, condition variables allow threads to wait in
 * a race-free way for arbitrary conditions to occur. Before a condition
 * variable is used, it must first be initialized. A condition variable,
 * represented by the pthread_cond_t data type, can be initialized in two
 * ways. We can assign the constant PTHREAD_COND_INITILIZER to a
 * statically-allocated condition variable, but if the condition variable
 * is allocated dynamically, we can use the pthread_cond_init() function
 * to initialize it.
 * We can use the pthread_mutex_destroy() function to deinitialize a
 * condition variable before freeing its underlying memory.
 * #include <pthread.h>
 * int pthread_cond_init(pthread_cond_t *cond, pthread_condattr_t attr);
 * int pthread_cond_destroy(pthread_cond_t *cond);
 *				Both return: 0 if 0K, error number on failure
 * Unless you need to create a conditional variable with nondefault
 * attributes, the attr argument to pthread_cond_init() can be set to NULL.
 *
 * We use pthread_cond_wait() to wait for a condition to be true. A variant
 * is provided to return an error code if the condition hasn't been
 * satisfied in the specified amount of time.
 * #include <pthread.h>
 * int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
 * int pthread_cond_timedwait(pthread_cond_t *cond,
 * 			pthread_mutex_t *mutex, const struct timespec *timeout);
 * 			Both return: 0 if OK, error number on failure
 * The mutex passed to pthread_cond_wait() protects the condition. The
 * caller passes it locked to the function, which then atomically places
 * the calling thread on the list of threads waiting for the condition and
 * unlocks the mutex. This closes the window between the time that the
 * condition is checked and the time that the thread goes to sleep waiting
 * for the condition to change, so that the thread doesn't miss a change
 * in the condition. When pthread_cond_wait() returns, the mutex is again
 * locked. 这个mutex变量是用来确保线程已经阻塞后,其他线程才获取到锁并发信号
 *
 * The pthread_cond_timedwait() function works the same as the
 * pthread_cond_wait() function with the addtion of the timeout. The timeout
 * value specifies how long we will wait. Using the struct timespec
 * structure, we need to specify how long we are willing to wait as an
 * absolute time instead of a relative time. For example, if we are willing
 * to wait 3 minutes, instead of translating 3 minutes into a timespec
 * structure, we need to translate now+3 minutes into a timespec structure.
 * We can use gettimeofday() to get the current time expressed as a timeval
 * structure and translate this into a timespec structure
 * structure, we need to specify how long we are willing to wait as an
 * absolute time instead of a relative time. For example, if we are willing
 * to wait 3 minutes, instead of translating 3 minutes into a timespec
 * structure, we need to translate now+3 minutes into a timespec structure.
 * We can use gettimeofday() to get the current time expressed as a timeval
 * structure and translate this into a timespec structure.
 *
 * If the timeout expires without the condition occurring,
 * pthread_cond_timedwait() will reacquire the mutex and return the error
 * ETIMEDOUT. When it returns from a successful call to pthread_cond_wait()
 * or pthread_cond_timedwait(), a thread needs to reevaluate the condition,
 * since another thread might have run and already changed the condition.
 * We protect the condition with a mutex and evaluate the condition in a
 * while loop.
 *
 * There are two functions to notify threads that a condition has been
 * satisfied. The pthread_cond_signal() function will wake up one thread
 * waiting on a condition, whereas the pthread_cond_broadcast() function
 * will wake up all threads waiting on a condition.
 * The POSIX specification allows for implementations of pthread_cond_signal
 * to wake up more than one thread, to make the implementation simpler.
 * #include <pthread.h>
 * int pthread_cond_signal(pthread_cond_t *cond);
 * int pthread_cond_broadcast(pthread_cond_t *cond);
 * 				Both return: 0 if OK, error number on failure
 * The pthread_cond_signal() function shall unblock at least one of the
 * threads that are blocked on the specified condition variable cond.
 */
int main(void)
{
	pthread_t tid1, tid2;
	int err;
	struct timespec ts;

	err = pthread_create(&tid1, NULL, thr_rtn1, NULL);
	if (err != 0) {
		printf("create thread 1 error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0) {
		printf("create thread 2 error: %s\n", strerror(err));
		return 1;
	}

	/* 一开始写这个程序时,将下面的maketimeout()语句写在while循环的外面,
	 * 执行的时候发现会打印出很多个下面的"time out"语句.原因分析如下:
	 * pthread_cond_timewait()函数等待的是绝对时间,而不是相对时间,在第一次
	 * 超时后,如果没有更新ts变量,则它保存的绝对时间比起当前时间要早,则后面
	 * 再在while循环中等待的时候,会立即超时,不断的打印超时信息.
	 * 所以,要将maketimeout()语句写在while循环里面,每次超时后,就更新ts变量.
	 */
	// maketimeout(&ts, 1);
	pthread_mutex_lock(&mutex);
	while (state == 0) {
		/* 在while循环里面更新ts变量的值,每次超时后,将绝对时间再加1秒 */
		maketimeout(&ts, 1);
		printf("main thread: while (state == 0)\n");
		err = pthread_cond_timedwait(&cond, &mutex, &ts);
		if (err == ETIMEDOUT) {
			printf("main thread: pthread_cond_timewait: time out\n");
		}
	}
	printf("main thread: wakeup..........\n");
	pthread_mutex_unlock(&mutex); 
	/* 调用pthread_join()等待线程2结束,防止主线程被先唤醒时,执行下面的
	 * return 0语句,终止整个进程,此时,线程2可能还没有被唤醒.
	 */
	pthread_join(tid2, NULL);
	return 0;
}
