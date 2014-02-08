#include <stdio.h>
#include <string.h>
#include <pthread.h>

/* Reader-write locks also have attributes, similar to mutexes. We use
 * pthread_rwlockattr_init() to initialize a pthread_rwlockattr_t structure
 * and pthread_rwlockattr_destroy() to deinitialize the structure.
 * #include <pthread.h>
 * int pthread_rwlockattr_init(pthread_rwlockattr_t *attr);
 * int pthread_rwlockattr_destroy(pthread_rwlockattr_t *attr);
 * 					Both return: 0 if OK, error number on failure
 * The only attribute supported for reader-writer locks is the process-
 * shared attribute. It is identical to the mutex process-shared attribute.
 * Just as with the mutex process-shared attributes, a pair of functions is
 * provided to get and set the process-shared attributes of read-writer
 * locks.
 * #include <pthread.h>
 * int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *a, int *p);
 * int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *attr, int p);
 * 					Both return: 0 if OK, error number on failure
 * Although POSIX defines only one reader-writer lock attribute,
 * implementations are free to define additional, nonstandard ones.
 */
int main(void)
{
	pthread_rwlockattr_t attr;
	int err, rwlock_pshared;

	err = pthread_rwlockattr_init(&attr);
	if (err != 0) {
		printf("pthread_rwlock_init error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_rwlockattr_getpshared(&attr, &rwlock_pshared);
	if (err != 0) {
		printf("pthread_rwlockattr_getpshared error: %s\n", strerror(err));
		return 2;
	}

	printf("rwlock_pshared: %d, ", rwlock_pshared);
	if (rwlock_pshared == PTHREAD_PROCESS_PRIVATE)
		printf("PTHREAD_PROCESS_PRIVATE\n");
	else if (rwlock_pshared == PTHREAD_PROCESS_SHARED)
		printf("PTHREAD_PROCESS_SHARED\n");
	else
		printf("**UNKNOWN**\n");

	err = pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_SHARED);
	if (err != 0) {
		printf("pthread_rwlockattr_setpshared error: %s\n", strerror(err));
		return 2;
	}

	err = pthread_rwlockattr_getpshared(&attr, &rwlock_pshared);
	if (err != 0) {
		printf("pthread_rwlockattr_getpshared error: %s\n", strerror(err));
		return 2;
	}

	printf("after set: rwlock_pshared: %d, ", rwlock_pshared);
	if (rwlock_pshared == PTHREAD_PROCESS_PRIVATE)
		printf("PTHREAD_PROCESS_PRIVATE\n");
	else if (rwlock_pshared == PTHREAD_PROCESS_SHARED)
		printf("PTHREAD_PROCESS_SHARED\n");
	else
		printf("**UNKNOWN**\n");

	return 0;
}
