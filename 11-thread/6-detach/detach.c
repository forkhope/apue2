#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

void *thr_rtn(void *arg)
{
	int err;
	printf("Enter thread 1: %u\n", (unsigned int)pthread_self());
	err = pthread_detach(pthread_self());
	if (err != 0)
		printf("detach pthread_self() error: %s\n", strerror(err));
	sleep(2);
	return (void *)0;
}

/* By default, a thread's termination status is retained until
 * pthread_join() is called for that thread. A thread's underlying storage
 * can be reclaimed immediately on termination if that thread has been
 * detached. When a thread is detached, the pthread_join() function can't
 * be used to wait for its termination status. A call to pthread_join() for
 * a detachecd thread will fail, returning EINVAL. We can detach a thread by
 * calling pthread_join().
 * #include <pthread.h>
 * int pthread_detach(pthread_t tid);
 * 				Returns: 0 if OK, error number on failure
 * We can create a thread that is already in the detached state by
 * modifyting the thread attributes we pass to pthread_create().
 */
int main(void)
{
	pthread_t tid;
	int err;

	err = pthread_create(&tid, NULL, thr_rtn, NULL);
	if (err != 0)
		printf("create thread 1 error: %s\n", strerror(err));
	
	sleep(1);
	err = pthread_join(tid, NULL);
	if (err != 0)
		printf("can't join with thread 1: %s\n", strerror(err));
	exit(0);
}
