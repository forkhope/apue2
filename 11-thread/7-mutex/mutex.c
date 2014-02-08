#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

pthread_mutex_t *mutexp;
int number;

void *thr_rtn1(void *arg)
{
	int i;

	/* This mutual-exclusion mechanism works only if we design our threads
	 * to follow the same data-access rules. The operating system doesn't
	 * serialize access to data for us. If we allow one thread to access a
	 * shared resource without first acquiring a lock, then inconsistencies
	 * can occur even though the rest of our threads do acquire the lock
	 * before attempting to access the shared resource.
	 *
	 * 线程1开始的时候没有对mutexp加锁,就开始读写number,此时虽然线程2已经
	 * 对mutexp加锁,线程1还是能读写到number变量.程序运行结果也说明这一点.
	 */
	for (i = 0; i < 3; ++i) {
		number = i;
		printf("b usleep(): thread 1: i=%d, number=%d\n", i, number);
		usleep(1000);
		printf("a usleep(): thread 1: i=%d, number=%d\n", i, number);
	}

	printf("thread 1 try to lock the mutex~~~~~~~\n");

	/* 当线程1试图对mutexp加锁后,它就会陷入阻塞,在线程2解锁之前,它获取不到
	 * 锁,就不会被唤醒,从而不能执行后面读写number变量的语句.
	 */
	pthread_mutex_lock(mutexp);
	printf("thread 1 locks the mutex~~~~~~~\n");
	for (i = 10; i < 13; ++i) {
		number = i;
		printf("b usleep(): thread 1: i=%d, number=%d\n", i, number);
		usleep(1000);
		printf("a usleep(): thread 1: i=%d, number=%d\n", i, number);
	}
	pthread_mutex_unlock(mutexp);

	printf("unlock mutex and leave thread 1.....\n");
	return (void *)0;
}

void *thr_rtn2(void *arg)
{
	int i;

	/* 线程2会一开始就对mutexp加锁,然后读写全局变量number,而线程1一开始没
	 * 有对mutexp加锁,就读写全局变量number.此时,虽然线程2对mutexp加了锁,
	 * 但是线程1还是可以读写到number.原因如上所述.可以理解为,对mutexp加锁,
	 * 锁的是mutexp本身,而不是锁pthread_mutex_lock(),pthread_mutex_unlock()
	 * 之间的代码,其他线程是可以不理会这个mutexp锁,直接访问到这段代码的.只
	 * 有当其他线程遵守锁机制,试图对mutexp再加锁时,才会被阻塞,从而实现对数
	 * 据的单一访问.可谓是,"只防君子,不防小人".
	 */
	pthread_mutex_lock(mutexp);
	printf("THREAD 2 LOCKS THE MUTEX-------\n");
	for (i = 20; i < 25; ++i) {
		number = i;
		printf("B USLEEP(): THREAD 2: i=%d, NUMBER=%d\n", i, number);
		usleep(1000);
		printf("A USLEEP(): THREAD 2: i=%d, NUMBER=%d\n", i, number);
	}
	pthread_mutex_unlock(mutexp);

	printf("UNLOCK MUTEX AND LEAVE THREAD 2.....\n");
	return (void *)0;
}

/* A mutex is basically a lock that we set (lock) before accessing a shared
 * resource and release (unlock) when we're done. While it is set, any other
 * thread that tries to set it will block until we release it.
 * If more than one thread is blocked when we unlock the mutex, then all
 * threads blocked on the lock will be make runnable, and the first one to
 * run will be able to set the lock. The others will see that the mutex is
 * still locked and go back to waiting for it to become available agin. In
 * this way, only one thread will proceed at a time. 即,当有多个线程阻塞时,
 * 它们获得锁的顺序不是按照阻塞的先后顺序来的.假设线程1先阻塞,线程2再阻塞,
 * 线程3最后阻塞,当锁被解开后,并不一定是先阻塞的线程1能获取到锁,而是哪个线
 * 程先运行,哪个线程就获取到锁,也就是取决于线程调度顺序.
 *
 * A mutex variable is represented by the pthread_mutex_t data type. Before
 * we can use a mutex variable, we must first initialize it by either
 * setting it to the constant PTHREAD_MUTEX_INITIALIZER (for statically-
 * allocated mutexes only) or calling pthread_mutex_init(). If we allocate
 * the mutex dynamically (by calling malloc(), for example), then we need
 * to call pthread_mutex_destroy() before freeing the memory.
 * #include <pthread.h>
 * int pthread_mutex_init(pthread_mutex_t *mutex,
 * 				const pthread_mutexattr_t *attr);
 * int pthread_mutex_destroy(pthread_mutex_t *mutex);
 * 				Both return: 0 if OK, error number on failure
 * To initialize a mutex with the default attributes, we set attr to NULL.
 *
 * The pthread_mutex_destroy() function shall destroy the mutex object
 * referenced by mutex; the mutex object becomes, in effect, uninitialized.
 * It shall be safe to destroy an initialized mutex that is unlocked.
 *
 * To lock a mutex, we call pthread_mutex_lock(). If the mutex is already
 * locked, the calling thread will block until the mutex is unlocked. To
 * unlock a mutex, we call pthread_mutex_unlock().
 * #include <pthread.h>
 * int pthread_mutex_lock(pthread_mutex_t *mutex);
 * int pthread_mutex_trylock(pthread_mutex_t *mutex);
 * int pthread_mutex_unlock(pthread_mutex_t *mutex);
 * 				All return: 0 if OK, error number on failure
 * If a thread can't afford to block, it can use pthread_mutex_trylock() to
 * lock the mutex conditionally. If the mutex is unlocked at the time
 * pthread_mutex_trylock() is called, then pthread_mutex_trylock() will lock
 * the mutex without blocing and return 0. Otherwise,pthread_mutex_trylock()
 * will fail, returning EBUSY without locking the mutex.
 */
int main(void)
{
	int err;
	pthread_t tid1, tid2;
	
	if ((mutexp = malloc(sizeof(pthread_mutex_t))) == NULL) {
		printf("malloc a pthread_mutex_t error\n");
		return 1;
	}

	err = pthread_mutex_init(mutexp, NULL);
	if (err != 0) {
		printf("pthread_mutex_init error: %s\n", strerror(err));
		return 2;
	}

	err = pthread_create(&tid1, NULL, thr_rtn1, NULL);
	if (err != 0) {
		printf("create thread 1 error: %s\n", strerror(err));
		return 3;
	}

	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0) {
		printf("create thread 2 error: %s\n", strerror(err));
		return 3;
	}

	/* 虽然这里先等待线程1结束,再等待线程2结束,但这并不代表线程1就一定会比
	 * 线程2先结束.实际上,线程1和线程2哪个会先结束,是不确定的.主线程在执行到
	 * 后面的pthread_join(tid2,NULL)时,线程2可能已经结束了,此时,pthread_join
	 * 会立即返回,不要因为代码语句的书写顺序而产生线程1会先结束的误解.
	 */
	err = pthread_join(tid1, NULL);
	if (err != 0) {
		printf("can't join with thread 1: %s\n", strerror(err));
		return 4;
	}

	err = pthread_join(tid2, NULL);
	if (err != 0) {
		printf("can't join with thread 2: %s\n", strerror(err));
		return 4;
	}

	/* 在释放mutexp指向的内存之前,需要先调用pthread_mutex_destroy()函数 */
	pthread_mutex_destroy(mutexp);
	free(mutexp);

	return 0;
}
