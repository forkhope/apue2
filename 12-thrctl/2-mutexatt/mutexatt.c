#include <stdio.h>
#include <string.h>
#include <pthread.h>

/* We use pthread_mutexattr_init() to initialize a pthread_mutexattr_t()
 * structure and pthread_mutexattr_destroy() to deinitialize one.
 * #include <pthread.h>
 * int pthread_mutexattr_init(pthread_mutexattr_t *attr);
 * int pthread_mutex_attr_destroy(pthread_mutexattr_t *attr);
 * 					Both return: 0 if OK, error number on failure
 * The pthread_mutexattr_init() function will initialize the pthread_mutex_t
 * structure with the default mutex attributes. Two attributes of interest
 * are the process-shared attribute and the type attribute. Within POSIX.1,
 * the process-shared attribute is optional; Within a process, multiple
 * threads can access the same synchronization object. This is the default
 * behavior. In this case, the process-shared mutex attribute is set to
 * PTHREAD_PROCESS_PRIVATE.
 * Access to shared data by multiple processes usually requires
 * synchronization, just as does access to shared data by multiple threads.
 * If the process-shared mutex attribute is set to PTHREAD_PROCESS_SHARED,
 * a mutex allocated from a memory extent shared between multiple processes
 * may be used for synchronization by those processes.
 *
 * We can use the pthread_mutexattr_getpshared() function to query a
 * pthread_mutexattr_t structure for its process-shared attribute. We can
 * change the process-shared attribute with the pthread_mutexattr_setpshared
 * function.
 * #include <pthread.h>
 * int pthread_mutexattr_getpshared(const pthread_mutexattr_t *attr, int p);
 * int pthread_mutexattr_setpshared(pthread_mutexattr_t *attr, int p);
 * 					Both return: 0 if OK, error number on failure
 *
 * The type mutex attribute controls the characteristics of the mutex.
 * POSIX.1 defines four types. The PTHREAD_MUTEX_NORMAL type is a standard
 * mutex that doesn't do any special error checking or deaklock detection.
 * The PTHREAD_MUTEX_ERRORCHECK mutex type provides error checking.
 * The PTHREAD_MUTEX_RECURSIVE mutex type allows the same thread to lock it
 * multiple times without first unlocking it. A recursive mutex maintains a
 * lock count and isn't released until it is unlocked the same number of
 * times it is locked. So if you lock a recursive mutex twice and then
 * unlock it, the mutex remains locked until it is unlocked a second time.
 * Finally, the PTHREAD_MUTEX_DEFAULT type can be used to request default
 * semantics. Implementations are free to map this to one of the other types
 *
 * We can use pthread_mutexattr_gettype() to get the mutex type attribute
 * and pthread_mutexattr_settype() to change the mutex type attribute.
 * #include <pthread.h>
 * int pthread_mutexattr_gettype(const pthread_mutexattr_t *attr, int type);
 * int pthread_mutexattr_settype(pthread_mutexattr_t *attr, int type);
 *					Both return: 0 if OK, error number on failure
 * 书中提到,pthread_cond_wait()和pthread_cond_timedwait()函数不要使用递归型
 * 的mutex,这两个函数内部自己解锁时,mutex还处于加锁状态.
 * 
 */
int main(void)
{
	pthread_mutex_t mutex;
	pthread_mutexattr_t mutexattr;
	int err;
	int pshared, type;

	err = pthread_mutexattr_init(&mutexattr);
	if (err != 0) {
		printf("Pthread_mutexattr_init error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_mutexattr_getpshared(&mutexattr, &pshared);
	if (err != 0) {
		printf("pthread_mutexattr_getpshard error: %s\n", strerror(err));
		return 2;
	}
	if (pshared == PTHREAD_PROCESS_PRIVATE)
		printf("default: pshared: PTHREAD_PROCESS_PRIVATE\n");
	else if (pshared == PTHREAD_PROCESS_SHARED)
		printf("default: pshared: PTHREAD_PROCESS_SHARED\n");

	err = pthread_mutexattr_gettype(&mutexattr, &type);
	if (err != 0) {
		printf("pthread_mutexattr_gettype error: %s\n", strerror(err));
		return 2;
	}
	if (type == PTHREAD_MUTEX_NORMAL)
		printf("default: type: PTHREAD_MUTEX_NORMAL\n");
	else if (type == PTHREAD_MUTEX_ERRORCHECK)
		printf("default: type: PTHREAD_MUTEX_ERRORCHECK\n");
	else if (type == PTHREAD_MUTEX_RECURSIVE)
		printf("default: type: PTHREAD_MUTEX_RECURSIVE\n");
	else if (type == PTHREAD_MUTEX_DEFAULT)
		printf("default: type: PTHREAD_MUTEX_DEFAULT\n");

	err = pthread_mutexattr_settype(&mutexattr, PTHREAD_MUTEX_ERRORCHECK);
	if (err != 0) {
		printf("pthread_mutexattr_settype error: %s\n", strerror(err));
		return 3;
	}
	pthread_mutex_init(&mutex, &mutexattr);
	pthread_mutex_lock(&mutex);
	err = pthread_mutex_lock(&mutex);
	if (err != 0)
		printf("pthread_mutex_lock twice error: %s\n", strerror(err));

	pthread_mutexattr_destroy(&mutexattr);
	pthread_mutex_destroy(&mutex);
	return 0;
}
