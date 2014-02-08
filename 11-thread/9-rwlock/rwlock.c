#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

int number = 50;
pthread_rwlock_t rwlock;

void *thr_rtn1(void *arg)
{
	int err;
	printf("before thread 1 get the read-lock\n");
	pthread_rwlock_rdlock(&rwlock);
	printf("after  thread 1 get the read-lock, number: %d\n", number);
	
	usleep(500);
	printf("thread 1 try to get the write lock\n");
	err = pthread_rwlock_trywrlock(&rwlock);
	if (err != 0)
		printf("since the rwlock has been read-locked, get write lock"
				"error: %s\n", strerror(err));
	
	printf("thread 1 release the read-lock\n");
	pthread_rwlock_unlock(&rwlock);

	usleep(1200);
	printf("thread 1 try to get the read  lock\n");
	err = pthread_rwlock_tryrdlock(&rwlock);
	if (err != 0)
		printf("since the rwlock has been write-locked, get read lock"
				"error: %s\n", strerror(err));
	pthread_exit((void *)0);
}

void *thr_rtn2(void *arg)
{
	int err;
	printf("before thread 2 get the read-lock\n");
	pthread_rwlock_rdlock(&rwlock);
	printf("after  thread 2 get the read-lock, number: %d\n", number);
	
	usleep(1000);
	printf("thread 2 release the read-lock\n");
	pthread_rwlock_unlock(&rwlock);
	printf("thread 2 want to get the write lock\n");
	err = pthread_rwlock_wrlock(&rwlock);
	if (err != 0)
		printf("since the rwlock has been read-locked, get write lock"
				"error: %s\n", strerror(err));
	else
		printf("thread 2 get the write lock!!!!!!!!\n");
	usleep(1000);
	pthread_rwlock_unlock(&rwlock);
	
	pthread_exit((void *)0);
}

/* With a mutex, the state is either locked or unlocked, and only one thread
 * can lock it at a time. Three states are possible with a reader-writer
 * lock: locked in read mode, locked in write mode, and unlocked. Only one
 * thread at a time can hold a reader-writer lock in write mode,but multiple
 * threads can hold a reader-writer lock in read mode at the same time.
 *
 * When a reader-writer lock is write-locked, all threads attempting to lock
 * it block until it is unlocked. When a reader-writer is read-locked, all
 * threads attempting to lock it in read mode are given access, but any
 * threads attempting to lock it in write mode block until all the threads
 * have relinquished their read locks.
 * 即,当读写锁是写模式时,不管试图加写锁还是加读锁都会被阻塞住.
 *
 * Reader-writer locks are well suited for situations in which data
 * structures are read more often than they are modified.
 *
 * As with mutexes, reader-writer locks must be initialized before use and
 * destroyed before freeing their underlying memory.
 * #include <pthread.h>
 * int pthread_rwlock_init(pthread_rwlock_t *rwlock,
 * 					const pthread_rwlockattr_t attr);
 * int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);
 * 					Both return: 0 if OK, error number on failure
 * A reader-writer lock is initialized by calling pthread_rwlock_init(). We
 * can pass a null pointer for attr if we want the reader-writer lock to
 * have the default attributes.
 * Before freeing the memory backing a reader-writer lock, we need to call
 * pthread_rwlock_destroy() to clean it up. If pthread_rwlock_init()
 * allocated any resources for the reader-writer lock,
 * pthread_rwlock_destroy() frees those resources. If we free the memeory
 * backing a reader-writer lock without first calling
 * pthread_rwlock_destroy(), any resources assigned to the lock will be lost
 *
 * To lock a reader-writer lock in read mode, we call pthread_rwlock_rdlock.
 * To writer-lock a reader-writer lock, we call pthread_rwlock_wrlock.
 * Regardless of how we lock a reader-writer lock, we can call
 * pthread_rwlock_unlock() to unlock it.
 * #include <pthread.h>
 * int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
 * 			All return: 0 if OK, error number on failure
 *
 * The Single UNIX Specification also defines conditional versions of
 * the reader-writer locking primitives.
 * #include <pthread.h>
 * int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
 * int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
 * 			Both return: 0 if OK, error number on failure
 */
int main(void)
{
	pthread_t tid1, tid2;
	int err;

	err = pthread_rwlock_init(&rwlock, NULL);
	if (err != 0) {
		printf("pthread_rwlock_init error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_create(&tid1, NULL, thr_rtn1, NULL);
	if (err != 0) {
		printf("pthread_create thread 1 error: %s\n", strerror(err));
		return 2;
	}

	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0) {
		printf("pthread_create thread 2 error: %s\n", strerror(err));
		return 2;
	}

	pthread_join(tid1, NULL);
	pthread_join(tid2, NULL);

	pthread_rwlock_unlock(&rwlock);
	err = pthread_rwlock_destroy(&rwlock);
	if (err != 0) {
		printf("pthread_rwlock_destroy error: %s\n", strerror(err));
		return 3;
	}

	return 0;
}
