#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void prepare(void)
{
	printf("preparing lock......\n");
	pthread_mutex_lock(&lock1);
	pthread_mutex_lock(&lock2);
	printf("prepare locked lock1 and lock2\n");
}

void parent(void)
{
	printf("parent unlocking locks.......\n");
	pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void child(void)
{
	printf("child unlocking locks.....\n");
	pthread_mutex_unlock(&lock1);
	pthread_mutex_unlock(&lock2);
}

void *thr_rtn(void *arg)
{
	printf("thread start\n");
	pause();
	return (void *)0;
}

void *thr_rtn2(void *arg)
{
	printf("Enter thread 2...\n");
	pthread_mutex_lock(&lock1);
	sleep(2);
	printf("thread 2 will unlock lock1\n");
	pthread_mutex_unlock(&lock1);
	printf("thread 2 unlock lock1\n");
	return (void *)0;
}

/* When a thread calls fork(), a copy of the entire process address space is
 * make for the child. By inheriting a copy of the address space, the child
 * also inherits the state of every mutex, reader-writer lock, and condition
 * variable from the parent process. If the parent consists of more than one
 * thread, the child will need to clean up the lock state if it isn't going
 * to call exec immediately after fork() returns.
 * Inside the child process, only one thread exists. It is made from a copy
 * of the thread that called fork() in the parent. Consider the case where
 * one thread has a mutex locked and the state covered by that mutex is
 * inconsistent while another thread calls fork(). In the child, the mutex
 * is in the locked state (locked by a nonexistent thread and thus can never
 * unclocked).
 * This problem can be avoided if the child calls one of the exec functions
 * directly after returning from fork(). In this case, the old address space
 * is discarded, so the lock state doesn't matter. This is not always
 * possible, however, so if the child needs to continue processing, we need
 * to use a different strategy.
 * To clean up the lock state, we can establish fork handlers by calling the
 * function pthread_atfork().
 * #include <pthread.h>
 * int pthread_atfork(void (*prepare)(void), void (*parent)(void),
 * 						void (*child)(void));
 * 						Returns: 0 if OK, error number on failure
 * With pthread_atfork(), we can install up to three functions to help clean
 * up the locks. The prepare fork handler is called in the parent before
 * fork() creates the child process. This fork handler's job is to acquire
 * all locks defined by the parent. The parent fork handler is called in
 * the context of the parent after fork() has created the child process, but
 * before fork() has returned. This fork handler's job is to unlock all the
 * locks acquired by the prepare fork handler. The child fork handler is
 * called in the context of the child process before returning from fork().
 * Like the parent fork handler, the child fork handler must to release all
 * the locks acquired by the prepare fork handler.
 * man pthread_atfork()手册提到,pthread_atfork()函数的建议用法如下:
 * The expected usage is that the prepare handler acquires all mutex locks
 * and the other two fork handlers release them. 下面的程序就是这么做的.
 *
 * 由于写时复制技术的存在,pthread_atfork()函数并不是锁一次,解锁两次.
 * We can call pthread_atfork() multiple times to install more than one set
 * of fork handlers. If we don't have a need to use one of the handlers, we
 * can pass a null pointer for the particular handler argument, and it will
 * have no effect. When multiple fork handlers are used, the order in which
 * the handlers are called differs. The parent and child fork handlers are
 * called in the order in which they were registered, whereas the prepare
 * fork handlers are called in the opposite order from which they were
 * registered.
 *
 * 书中Exercises 12.1提到,如果将这个程序的运行结果重定向到文件时,输出结果
 * 可能会输出两次,书中答案解释如下:
 * This is not a multithreading problem, as one might first guess. The
 * standard I/O routines are indeed thread-safe. When we call fork(), each
 * process gets a copy of the standard I/O data structures. When we run the
 * program with standard output attached to a terminal, the output is line
 * buffered, so every time we print a line, the standard I/O library writes
 * it to our terminal. However, if we redirect the standard output to a
 * file, then the standard output is fully buffered. The output is written
 * when the buffer fills or the process closes the stream. When we fork() in
 * this example, the buffer contains several printed lines not yet written,
 * so when the parent and the child finally flush their copies of the
 * buffer, the initial duplicate contents are written to the file.
 */
int main(void)
{
	int err;
	pid_t pid;
	pthread_t tid, tid2;

	pthread_create(&tid2, NULL, thr_rtn2, NULL);
	sleep(1);

	/* pthread_atfork()函数可以清除锁状态的原因是,其prepare函数一般会先对
	 * 锁加锁,如果此时该锁被其他线程锁住了,fork()函数会因获取不到锁而阻塞,
	 * 执行其他线程解锁后,fork()函数才能获取到锁,然后它又执行parent和child
	 * 函数,解开刚才加的锁,由于写时复制,所以父子进程都执需要行解锁操作.
	 * 从执行结果中,可以看到这一点.线程2会先对lock1加锁,然后fork的时候,执行
	 * prepare()函数,可以看到线程2打印"thread 2 unlock lock1"语句后,prepare
	 * 函数才打印"prepare locked lock1 and lock2"语句,获取到锁.
	 * 即,为了避免fork之后,子进程继承到一个没有解开的锁,通过pthread_atfork
	 * 函数安装fork handler,让fork()函数执行prepare()函数试图加锁,从而阻塞,
	 * 直到父进程的线程解开所有的锁后,再真正开始fork.
	 */
	err = pthread_atfork(prepare, parent, child);
	if (err != 0) {
		printf("pthread_atfork error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_create(&tid, NULL, thr_rtn, NULL);
	if (err != 0) {
		printf("pthread_create error: %s\n", strerror(err));
		return 1;
	}

	printf("parent about to fork()...\n");
	pid = fork();
	if (pid < 0) {
		printf("fork error: %s\n", strerror(err));
		return 1;
	}
	else if (pid == 0) {	/* child */
		printf("child returned from fork\n");
	}
	else		/* parent */
		printf("parent returned from fork\n");

	return 0;
}
