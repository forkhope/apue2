#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

void printcondattr(pthread_condattr_t *condattr)
{
	int err, pshared;

	err = pthread_condattr_getpshared(condattr, &pshared);
	if (err != 0) {
		printf("pthread_condattr_getpshared error: %s\n", strerror(err));
		exit(3);
	}

	printf("condition attribute: pshared: %d, ", pshared);
	if (pshared == PTHREAD_PROCESS_PRIVATE)
		printf("PTHREAD_PROCESS_PRIVATE\n");
	else if (pshared == PTHREAD_PROCESS_SHARED)
		printf("PTHREAD_PROCESS_SHARED\n");
	else
		printf("**UNKNOWN**\n");
}

/* Condition variables have attributes, too. There is a pair of functions
 * for initializing and deinitializing them, similar to mutexes and
 * reader-writer locks.
 * #include <pthread.h>
 * int pthread_condattr_init(pthread_condattr_t *attr);
 * int pthread_condattr_destroy(pthread_condattr_t *attr);
 * 					Both return: 0 if OK, error number on failure
 * Just as with the other synchronization primitives, condition variables
 * support the process-shared attribute.
 * #include <pthread.h>
 * int pthread_condattr_getpshared(const pthread_condattr_t *a, int *p);
 * int pthread_condattr_setpshared(pthread_condattr_t *a, int pshared);
 * 				Both return: 0 if OK, error number on failure
 */
int main(void)
{
	pthread_condattr_t condattr;
	int err;

	err = pthread_condattr_init(&condattr);
	if (err != 0) {
		printf("pthread_condattr_init error: %s\n", strerror(err));
		return 1;
	}

	printcondattr(&condattr);

	err = pthread_condattr_setpshared(&condattr, PTHREAD_PROCESS_SHARED);
	if (err != 0) {
		printf("pthread_condattr_setpshared error: %s\n", strerror(err));
		return 2;
	}
	
	printcondattr(&condattr);

	pthread_condattr_destroy(&condattr);

	return 0;
}
