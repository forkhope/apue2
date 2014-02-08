#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>

sigset_t mask;
int quitflag = 0;	/* set nonzero by thread */

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *thr_rtn(void *arg)
{
	int err, signo;

	/* Note that we block SIGINT and SIGQUIT in the beginning of the main
	 * thread. When we create the thread to handle signals, the thread
	 * inherits the current signal mask. Since sigwait() will unblock the
	 * signals, only one thread is available to receive signals. This
	 * enables us to code the main thread with out having to worry about
	 * interrupts from those signals.
	 */
	for (;;) {
		err = sigwait(&mask, &signo);
		if (err != 0) {
			printf("thread 1: sigwait error: %s\n", strerror(err));
			exit(1);
		}

		/* We change the value of quitflag under the protection of a mutex
		 * so that the main thread of control can't miss the wake-up call
		 * made when we call pthread_cond_signal(). We use the same mutex
		 * in the main thread of control to check the value of the flag,
		 * and atomically release the mutex and wait for the condition.
		 */
		switch (signo) {
			case SIGINT:
				printf("thread 1: interrupt\n");
				break;
			case SIGQUIT:
				pthread_mutex_lock(&mutex);
				quitflag = 1;
				pthread_mutex_unlock(&mutex);
				pthread_cond_signal(&cond);
				return ((void *)0);
			default:
				printf("unexpected signal %d\n", signo);
				exit(1);
		}
	}
	return ((void *)0);
}

/* Each thread its own signal mask, but the signal disposition is shared
 * by all threads in the process. This means that individual threads can
 * block signals, but when a thread modifies the action associated with a
 * given signal, all thread share the action.
 * Signals are delivered to a single thread in the process. 
 *
 * The behavior of sigprocmask() is undefined in a multithreaded process.
 * Threads have to use pthread_sigmask() instead.
 * #include <signal.h>
 * int pthread_sigmask(int how, const sigset_t *set, sigset_t *oset);
 * 				Returns: 0 if OK, error number on failure.
 * The pthread_sigmask() function is identical to sigprocmask(), except that
 * pthread_sigmask() works with threads and returns an error code on failure
 * instead of setting errno and returning -1.
 *
 * A thread can wait for one or more signals to occur by calling sigwait().
 * #include <signal.h>
 * int sigwait(const sigset_t *set, int *signop);
 * 				Returns: 0 if OK, error number on failure
 * The set argument specifies the set of signals for which the thread is
 * waiting. On return, the integer to which signop points will contain the
 * number of the signal that was delivered. signop中保存的是信号码的值,而不
 * 是返回所等待到的信号数目,sigwait()在接收到第一个信号时就会返回了.  *
 * If one of the signals specified in the set is pending at the time sigwait
 * is called, then sigwait() will return without blocking. Before returning,
 * sigwait() removes the signal from the set of signals pending for the
 * process. To avoid erroneous behavior, a thread must block the signals it
 * is waiting for before calling sigwait(). The sigwait() function will
 * atomically unblock the signals and wait until one is delivered. Before
 * returning, sigwait() will restore the thread's signal mask. If the
 * signals are not blocked at the time that sigwait() is called, then a
 * timing window is opened up where one of the signals can be delivered to
 * the thread before it completes its call to sigwait(). 即,在sigwait()函数
 * 进入阻塞之前,它会解开对set参数所指定信号的阻塞(这些信号可能之前已经被阻塞
 * 也可能没有被阻塞),当sigwait()函数执行结束后,它又会恢复执行该函数之前的信
 * 号掩码.为了避免在调用sigwait()函数之前,没有被阻塞的信号已经提前递送,要求
 * 在调用sigwait()函数之前,先阻塞要等待的信号,调用sigwait()函数之后,sigwait
 * 内部会自动解开对这些信号的阻塞,并等待它们发生,函数退出后,会重新阻塞信号.
 * 阻塞的信号队列是对整个进程而言的,每个线程有自己的信号掩码,但是当前被阻塞
 * 的信号集合是所有线程共享的.下面提到如果多个线程等待同一个阻塞信号时的处理
 *
 * If multiple threads are blocked in calls to sigwait() for the same signal
 * only one of the threads will return from sigwait() when the signal is
 * delivered. If a signal is being caught (the process has established a
 * signal handler by using sigaction(), for example) and a thread is waiting
 * for the same signal in a call to sigwait(), it is left up to the
 * implementation to decide which way to deliver the signal. In the case,
 * the implementation could either allow sigwait() to return or invoke the
 * signal handler, but not both.
 *
 * To send a signal to a process, we call kill(), To send a signal to a
 * thread, we call pthread_kill().
 * #include <signal.h>
 * int pthread_kill(pthread_t thread, int signo);
 * 					Returns: 0 if OK, error number failure
 * We can pass a signo value of 0 to check for existence of the thread. If
 * the default action for a signal is to terminate the process, then sending
 * the signal to a thread will still kill the entire process.
 * Note that alarm timers are a process resource, and all threads share the
 * same set of alarms. Thus, it is not possible for multiple threads in a
 * process to use alarm timers without interfering (or cooperating) with one
 * another.
 */
int main(void)
{
	pthread_t tid;
	sigset_t oldmask;
	int err;

	sigemptyset(&mask);
	sigaddset(&mask, SIGINT);
	sigaddset(&mask, SIGQUIT);
	err = pthread_sigmask(SIG_BLOCK, &mask, &oldmask);
	if (err != 0) {
		printf("pthread_sigmask error: %s\n", strerror(err));
		return 1;
	}

	/* 上面提到,创建线程后,新创建的线程将会继承当前线程的信号掩码 */
	err = pthread_create(&tid, NULL, thr_rtn, NULL);
	if (err != 0) {
		printf("pthread_create error: %s\n", strerror(err));
		return 2;
	}

	pthread_mutex_lock(&mutex);
	/* 下面的pthread_cond_wait()语句并不是必须的,但是如果此时主线程什么都
	 * 不做,就是等待quitflag变为真的话,调用pthread_cond_wait()会让主线程
	 * 休眠,节省一点cpu资源.
	 */
	while (quitflag == 0)
		pthread_cond_wait(&cond, &mutex);
	pthread_mutex_unlock(&mutex);

	/* SIGQUIT has been caught and is now blocked; do whatever.
	 * 在之前sigwait()之前,调用pthread_sigwait()函数阻塞了SIGINT, SIGQUIT.
	 * 线程1中的sigwait()函数执行过程中,会解开对SIGINT和SIGQUIT的阻塞.现在
	 * sigwait()执行结束,SIGQUIT, SIGINT又重新被阻塞,如果不想继续保持这种
	 * 阻塞,执行下面的语句恢复原先的信号掩码.
	 */
	quitflag = 0;

	/* reset signal mask which unblocks SIGQUIT */
	err = pthread_sigmask(SIG_SETMASK, &oldmask, NULL);
	if (err != 0) {
		printf("SIG_SETMASK error: %s\n", strerror(err));
		return 1;
	}

	return 0;
}
