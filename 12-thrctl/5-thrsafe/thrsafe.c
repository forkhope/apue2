#include <stdio.h>
#include <linux/limits.h>	// 包含ARG_MAX的声明,书中写的是<limits.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>

static char envbuf[ARG_MAX];
extern char **environ;

/* A nonreentrant version of getenv(). This version is not reentrant. If
 * two threads call it at the same time, they will see inconsistent results,
 * because the string returned is stored in a single static buffer that is
 * shared by all threads calling getenv().
 */
char *getenv_l(const char *name)
{
	int len, i;

	len = strlen(name);
	for (i = 0; environ[i] != NULL; ++i) {
		if (strncmp(name, environ[i], len) == 0
				&& environ[i][len] == '=') {
			strcpy(envbuf, &environ[i][len+1]);
			return envbuf;
		}
	}
	return NULL;
}

pthread_mutex_t envmutex;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static void thread_init(void)
{
	pthread_mutexattr_t mutexattr;

	pthread_mutexattr_init(&mutexattr);
	/* 之前在UBUNTU上测试,下面的写法不加任何编译选项都是可以的,但是在
	 * debian6上会报警报错,可能是linux或者gcc前后版本不兼容导致的:
	 * warning: implicit declaration of function 'pthread_mutexattr_settype'
	 * error: 'PTHREAD_MUTEX_RECURSIVE' undeclared 
	 * 解决方法是添加 -D_GNU_SOURCE 编译选项.
	 *
	 * 关于要将envmutex设为PTHREAD_MUTEX_RECURSIVE的原因,书中给了解释:
	 * If we make getenv_r() thread-safe, that doesn't mean that it is
	 * reentrant with respect to signal handlers. If we use a nonrecursive
	 * mutex, we run the risk that a thread will deaklock itself if it calls
	 * getenv_t() from a signal handler. Thus, we must use a recussive mutex
	 * to prevent other threads from changing the data structures while we
	 * look at them, and also prevent deadlocks from signal handler.
	 */
	pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(&envmutex, &mutexattr);
	pthread_mutexattr_destroy(&mutexattr);
}

/* A reentrant (thread-safe) version of getenv(). This version uses the
 * pthread_once() function to ensure that the thread_init() function is
 * called only once per process.
 */
int getenv_r_l(const char *name, char *buf, size_t buflen)
{
	int i, len, ret;

	/* int pthread_once(pthread_once_t *once_ctrl, void (*init_rtn)(void));
	 * The first call to pthread_once() by any thread in a process, with a
	 * given once_ctrl, shall call the init_rtn() with no arguments. 
	 * Subsequent calls of pthread_once() with the same once_ctrl shall not
	 * call the init_rtn().
	 */ 
	pthread_once(&once, thread_init);
	len = strlen(name);
	pthread_mutex_lock(&envmutex);
	for (i = 0; environ[i] != NULL; ++i) {
		if (strncmp(name, environ[i], len) == 0
				&& environ[i][len] == '=') {
			if (strlen(&environ[i][len+1]) >= buflen) {
				ret = ENOSPC;
			}
			strcpy(buf, &environ[i][len+1]);
			ret = 0;
		}
	}
	pthread_mutex_unlock(&envmutex);
	ret = ENOENT;
	return ret;
}

void *thr_rtn1(void *arg)
{
	char buf[1024];

	printf("Enter thread 1 ......\n");
	printf("THREAD 1: getenv_l(USER): %s\n", getenv_l("USER"));
	getenv_r_l("USER", buf, 1024);
	printf("THREAD 1: getenv_r_l(USER): %s\n", buf);
	pthread_exit((void *)0);
}

void *thr_rtn2(void *arg)
{
	char buf[1024];

	printf("Enter thread 2 ......\n");
	printf("THREAD 2: getenv_l(USER): %s\n", getenv_l("USER"));
	getenv_r_l("USER", buf, 1024);
	printf("THREAD 2: getenv_r_l(USER): %s\n", buf);
	pthread_exit((void *)0);
}

/* If a function can be safely called by multiple threads at the same time,
 * we say that the function is thread-safe. 书中列举了非线程安全的函数.
 * Implementations that support thread-safe functions will define the
 * _POSIX_THREAD_SAFE_FUNCTIONS symbol in <unistd.h>. Applications can also
 * use the _SC_THREAD_SAFE_FUNCTIONS argument with sysconf() to check for
 * support of thread_safe functions at runtime.
 * When it supports the thread-safe functions feature, an implementation
 * provides alternate, thread-safe versions of some of the POSIX.1 functions
 * that aren't thread-safe. Many functions are not thread-safe, because they
 * return data stored in a static memory buffer. They are made thread-safe
 * by changing their interfaces to require that the caller provide its own
 * buffer. 书中同样给出了对应的替代线程安全函数列表,如 readdir_r,ctime_r 等
 *
 * If a function is reentrant with respect to multiple threads, we say that
 * it is thread-safe. This doesn't tell us, however, whether the funcion is
 * reentrant with respect to signal handlers. We say that a function that is
 * safe to be reentered from an asynchronous signal handler is async-signal
 * safe.
 */
int main(void)
{
	pthread_t tid1, tid2;
	int err;

	err = pthread_create(&tid1, NULL, thr_rtn1, NULL);
	if (err != 0)
		return 1;

	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0)
		return 1;

	pthread_exit((void *)0);
}
