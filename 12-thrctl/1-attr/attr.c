#include <stdio.h>
#include <string.h>
#include <pthread.h>

void *thr_rtn(void *arg)
{
	printf("Enter thread 1.........\n");
	pthread_exit((void *)0);
}

void printdetachstate(const char *s, pthread_attr_t *attr)
{
	int state;

	pthread_attr_getdetachstate(attr, &state);
	printf("%s, the detach state is: %d", s, state);
	if (state == PTHREAD_CREATE_JOINABLE)
		printf(", equal to PTHREAD_CREATE_JOINABLE\n");
	else if (state == PTHREAD_CREATE_DETACHED)
		printf(", equal to PTHREAD_CREATE_DETACHED\n");
	else
		printf(", **unknown value**\n");
}

void printdefaultattr(pthread_attr_t *attr)
{
	size_t guardsize, stacksize;
	void *stackaddr = NULL;

	printdetachstate("default", attr);

	pthread_attr_getguardsize(attr, &guardsize);
	printf("pthread_attr_t: default guardsize: %lu\n", guardsize);

	pthread_attr_getstacksize(attr, &stacksize);
	printf("pthread_attr_t: default stacksize: %lu\n", stacksize);

	pthread_attr_getstack(attr, &stackaddr, &stacksize);
	printf("pthread_attr_t: default stacksize: %lu, stackaddr: %p\n",
			stacksize, stackaddr);
}

/* We can use the pthread_attr_t structure to modify the default attributes,
 * and associate these attributes with threads that we create. We use the
 * pthread_attr_init() function to initialize the pthread_attr_t structure.
 * After calling pthread_attr_init(), the pthread_attr_t structure contains
 * the default values for all the thread attributes supported by the
 * implementation. To change individual attributes, we need to call other
 * function.
 * #include <pthread.h>
 * int pthread_attr_init(pthread_attr_t *attr);
 * int pthread_attr_destroy(pthread_attr_t *attr);
 * 				Both return: 0 if OK, error number on failure.
 * To deinitialize a pthread_attr_t structure, we call pthread_attr_destroy.
 * If an implementation of pthread_attr_init() allocated any dynamic memory
 * for the attribute object, pthread_attr_destroy() will free that memory.
 * In addition, pthread_attr_destroy() will initialize the attribute object
 * with invalid values, so if it is used by mistake, pthread_create() will
 * return an error.
 * The pthread_attr_t structure is opaque to applications. This means that
 * applications aren't supposed to know anything about its internal
 * structure, thus promoting application portability. Following this model,
 * POSIX.1 defines separate functions to query and set each attribute.
 *
 * If we know that we don't need the thread's termination status at the time
 * we create the thread, we can arrange for the thread to start out in the
 * detached state modifying the detachstate thread attribute in the
 * pthread_attr_t structure. We can use the pthread_attr_setdetachstate()
 * function to set the detachstate thread attribute to one of two legal
 * values: PTHREAD_CREATE_DETACHED to start the thread in the detached state
 * or PTHREAD_CREATE_JOINABLE to start the thread normally, so its
 * termination status can be retrieved by the application.
 * #include <pthread.h>
 * int pthread_attr_getdetachstate(const pthread_attr_t *attr, int *state);
 * int pthread_attr_setdetachstate(pthread_attr_t *attr, int detachstate);
 * 				Both return: 0 if OK, error number on failure
 * We can call pthread_attr_getdetachstate() to obtain the current
 * detachstate attribute. The integer pointed to by the second argument is
 * set to either PTHREAD_CREATE_DETACHED or PTHREAD_CREATE_JOINABLE,
 * depending on the value of the attribute in the given pthread_attr_t
 * structure.
 *
 * POSIX.1 defines several interfaces to manipulate thread stack attributes.
 * Two older function, pthread_attr_getstackaddr() and
 * pthread_attr_setstackaddr(), are marked as obsolete in Version 3 of the
 * Single UNIX Specification, although many pthreads implementations still
 * provide them. The preferred way to query and modify a thread's stack
 * attributes is to use the newer functions pthread_attr_getstack() and
 * pthread_attr_setstack().
 * #include <pthread.h>
 * int pthread_attr_getstack(const pthread_attr_t *attr,
 * 					void **stackaddr, size_t *stacksize);
 * int pthread_attr_setstack(const pthread_attr_t *attr,
 * 					void *stackaddr, size_t *stacksize);
 * 					Both return: 0 if OK, error number on failure
 * These two functions are used to manage both the stackaddr and the
 * stacksize thread attributes.
 *
 * An application can also get the set the stacksize thread attribute using
 * the pthread_attr_getstacksize() and pthread_attr_setstacksize() functions
 * #include <pthread.h>
 * int pthread_attr_getstacksize(const pthread_attr_t *attr,size_t *size);
 * int pthread_attr_setstacksize(pthread_attr_t *attr, size_t stacksize);
 * 				Both return: 0 if OK, error number on failure
 * The pthread_attr_setstacksize() function is useful when you want to
 * change the default stack size but don't want to deal with allocating
 * the thread stacks on your own.
 *
 * The guardsize thread attrigute controls the size of the memory extent
 * after the end of the thread's stack to protect against stack overflow.
 * By default, this is set ot PAGESIZE bytes. We can set the guardsize
 * thread attribute to 0 to disable this feature: no guard buffer will be
 * provided in this case.
 * #include <pthread.h>
 * int pthread_attr_getguardsize(const pthread_attr_t *attr, size_t *size);
 * int pthread_attr_setguardsize(pthread_attr_t *attr, size_t guardsize);
 * 				Both return: 0 if OK, error number on failure
 */
int main(void)
{
	int err;
	pthread_t tid;
	pthread_attr_t attr;

	err = pthread_attr_init(&attr);
	if (err != 0) {
		printf("pthread_attr_init error: %s\n", strerror(err));
		return 1;
	}

	/* 打印pthread_attr_init()函数初始化的默认属性值 */
	printdefaultattr(&attr);

	err = pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	if (err != 0) {
		printf("set PTHREAD_CREATE_DETACHED error: %s\n", strerror(err));
		return 2;
	}
	printdetachstate("after detached", &attr);

	err = pthread_create(&tid, &attr, thr_rtn, NULL);
	if (err != 0) {
		printf("create thread 1 error: %s\n", strerror(err));
		return 3;
	}

	pthread_attr_destroy(&attr);

	/* It returns EINVAL, when thread is not a joinable thread. */
	err = pthread_join(tid, NULL);
	if (err != 0)
		printf("since detach, pthread_join error: %s\n", strerror(err));

	/* 不用 return 0,避免主线程先执行时,线程1还没有得到执行机会 */
	pthread_exit((void *)0);
}
