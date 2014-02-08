#include <stdio.h>
#include <string.h>
#include <linux/limits.h>	/* ARG_MAX */
#include <stdlib.h>			/* free() */
#include <pthread.h>

extern char **environ;

static pthread_key_t key;
static pthread_once_t once_done = PTHREAD_ONCE_INIT;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* We use pthread_once() to ensure that only one key is created for the
 * thread-specific data we will use. For the destructor function, we use
 * free() to free the memory previously allocated by malloc(). The
 * destructor function will be called with the value of the thread-specific
 * data only if the value is non-null.
 * #include <pthread.h>
 * pthread_once_t initflag = PTHREAD_ONCE_INIT;
 * int pthread_once(pthread_once_t *initflag, void (*initfn)(void));
 * 				Returns: 0 if OK, error number on failure
 * The initflag must be a nonlocal variable (i.e., global or static) and
 * initialized to PTHREAD_ONCE_INIT.
 * If each thread calls pthread_once(), the system guarantees that the
 * initialization routine, initfn, will be called only once, on the first
 * call to pthread_once().
 */
void thread_init(void)
{
	/* 下面的free 就是 free() 函数,当线程正常退出时,free()函数被调用,且参数
	 * 是该线程调用pthread_setspecific()函数所关联的线程私有数据的地址.
	 */
	pthread_key_create(&key, free);
}

/* A thread-safe, compatible version of getenv().
 * Note that although this version of getenv() is thread-safe, it is not
 * async-signal safe. Even if we made the mutex recursive, we could not
 * make it reentrant with respect to signal handlers, because if calls
 * malloc(), which itself is not async-signal safe.
 */
char *getenv_l(const char *name)
{
	int i, len;
	char *envbuf;

	pthread_once(&once_done, thread_init);
	pthread_mutex_lock(&mutex);

	/* 先调用pthread_getspecific()函数获取线程私有数据,判断该数据是否存在,
	 * 如果不存在,再调用malloc()和pthread_setspecific()函数关联线程私有数据.
	 * 这样,即是同一个线程多次调用getenv_l()函数时,线程私有数据也不会被多次
	 * 创建和关联.
	 */
	envbuf = (char *)pthread_getspecific(key);
	if (envbuf == NULL) {
		envbuf = malloc(ARG_MAX);
		if (envbuf == NULL) {
			pthread_mutex_unlock(&mutex);
			return NULL;
		}
		pthread_setspecific(key, envbuf);
	}

	len = strlen(name);
	for (i = 0; environ[i] != NULL; ++i) {
		if (strncmp(name, environ[i], len) == 0
				&& environ[i][len] == '=') {
			strcpy(envbuf, &environ[i][len+1]);
			pthread_mutex_unlock(&mutex);
			return envbuf;
		}
	}
	pthread_mutex_unlock(&mutex);
	return NULL;
}

void *thr_rtn1(void *arg)
{
	printf("Enter thread 1 .......\n");
	printf("thread 1: getenv_l(USER): %s\n", getenv_l("USER"));
	return (void *)0;
}

void *thr_rtn2(void *arg)
{
	printf("Enter thread 2 .......\n");
	printf("thread 2: getenv_l(USER): %s\n", getenv_l("USER"));
	return (void *)0;
}

/* Thread-specific data, also known as thread-private data, is a mechanism
 * for storing and finding data associated with a particular thread. The
 * reason we call the data thread-specific, or thread-private, is that we'd
 * like each thread to access its own separate copy of the data, without
 * worrying about synchronizing access with other threads.
 *
 * Before allocating thread-specific data, we need to create a key to
 * associate with the data. The key will be used to gain access to the
 * thread-specific data. We use pthread_key_create() to create a key.
 * #include <pthread.h>
 * int pthread_key_create(pthread_key_t *keyp, void (*destructor)(void *));
 * 					Returns: 0 if OK, errno number on failure
 * The key created is stored in the memory location pointed to by keyp. The
 * same key can used by all threads in the process, but each thread will
 * associate a different thread-specific data address with the key. When the
 * key is created, the data address for each thread is set to a null value.
 * In addition to creating a key, pthread_key_create() associates an
 * optional destructor function with the key. When the thread exits, if the
 * data address has been set to a non-null value, the destructor function is
 * called with the data address as the only argument. If destructor is null,
 * then no destructor function is associated with the key. When the thread
 * exits normally, by calling pthread_exit() or by returning, the destructor
 * is called. But if the thread calls exit, _exit, _Exit, or about, or
 * otherwise exits abnormally, the destructor is not called.
 *
 * We can break the association of a key with the thread-specific data
 * values for all threads by calling pthread_key_delete.
 * int pthread_key_delete(pthread_key_t *key);
 * 				Returns: 0 if OK, error number on failure
 * Note that calling pthread_key_delete() will not invoke the destructor
 * function associated with the key.
 *
 * Once a key is created, we can associate thread-specific data with the
 * key by calling pthread_setspecific(). We can obtain the address of the
 * thread-specific data with pthread_getspecific().
 * void *pthread_getspecific(pthread_key_t key);
 * 			Returns: thread-specific data value or NULL if no value has
 * 			         been associated with the key.
 * int pthread_setspecific(pthread_key_t key, const void *value);
 * 			Returns: 0 if OK, error number on failure
 * If no thread-specific data has been associated with a key,
 * pthread_getspecific() will return a null pointer. We can use this to
 * determine whether we need to call pthread_setspecific();
 */
int main(void)
{
	pthread_t tid1, tid2;

	tid1 = pthread_create(&tid1, NULL, thr_rtn1, NULL);
	if (tid1 != 0)
		return 1;

	tid2 = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (tid2 != 0)
		return 1;

	pthread_exit((void *)0);
}
