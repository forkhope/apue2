#include "apue.h"
#include <unistd.h>
#include <pthread.h>

pthread_t ntid;

void printids(const char *s)
{
	pid_t pid;
	pthread_t tid;

	pid = getpid();
	tid = pthread_self();
	printf("%s pid: %u, thead id: %u (0x%x)\n", s, (unsigned int)pid,
			(unsigned int)tid, (unsigned int)tid);
}

void *thr_fn(void *arg)
{
	printids("new thread: ");
	return (void *)0;
}

/* Additional threads can be created by calling the pthread_create function.
 * #include <pthread.h>
 * int pthread_create(pthread_t *restrict tidp,
 *					  const pthread_attr_t *restrict attr,
 *					  void *(*start_rtn)(void *), void *restrict arg);
 *			Returns: 0 if OK, error number on failure
 * The memory location pointed to by tidp is set to the thread ID of the
 * newly created thread when pthread_create() returns successfully. The
 * attr argument is used to customize various thread attributes. For now,
 * we'll set this to NULL to create a thread with the default attributes.
 * The newly created thread starts running at the address of the start_rtn
 * function. This function takes a single argument, arg, which is a
 * typeless pointer.
 * When a thread is created, there is no guarantee which runs first: the
 * newly created thread of the calling thread.
 *
 * Note that the pthread functions usually return an error code when they
 * fail. They don't set errno like the other POSIX functions.
 */
int main(void)
{
	int err;

	err = pthread_create(&ntid, NULL, thr_fn, NULL);
	if (err != 0)
		err_quit("pthread_create error: %s\n", strerror(err));
	printids("main thread: ");

	/* This example has two oddities, necessary to handle races between the
	 * main thread and the new thread. The first is the need to sleep in
	 * the main thread. If it doesn't sleep, the main thread might exit,
	 * thereby terminating the entire process before the new thread gets a
	 * chance to run. This behavior is dependent on the operating system's
	 * threads implementation and scheduling algorithms.
	 */
	sleep(1);
	exit(0);
}
