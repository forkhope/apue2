#include "apue.h"
#include <unistd.h>
#include <pthread.h>

struct foo {
	int a, b, c, d;
};

void printfoo(const char *s, const struct foo *fp)
{
	printf("%s\n", s);
	/* 测试用的机器是64位的,而在64位机器上,指针类型长度是64位,int型是32位.
	 * 所以(unsigned int)fp 将fp指针转换为unsigned int类型时,编译报警:
	 * 		warning: cast from pointer to integer of different size
	 * 此时,应将指针转换为unsigned long.这个规则也适用于32位机器.
	 */
	printf("	structure at: 0x%lx\n", (unsigned long)fp);
	printf("	foo.a: %d\n", fp->a);
	printf("	foo.b: %d\n", fp->b);
	printf("	foo.c: %d\n", fp->c);
	printf("	foo.d: %d\n", fp->d);
}

void *thr_rtn1(void *arg)
{
	struct foo f = {1, 2, 3, 4};

	printfoo("Thread 1 foo:", &f);
	pthread_exit((void *)&f);
}

void *thr_rtn2(void *arg)
{
	printf("Thread 2, ID is %u\n", (unsigned int)pthread_self());
	pthread_exit((void *)0);
}

/* Incorrect use of pthread_exit() argument.
 * If a thread allocates a structure on its stack and passes a pointer to
 * this structure to pthread_exit(), then the stack might be destroyed and
 * its memory reused for something else by the time the caller of
 * pthread_join() tries to use it.
 * To solve this problem, we can either use a global structure or allocate
 * the structure using mallock().
 */
int main(void)
{
	int err;
	pthread_t tid1, tid2;
	struct foo *fp;
	
	printf("Create thread 1 ......\n");
	err = pthread_create(&tid1, NULL, thr_rtn1, NULL);
	if (err != 0)
		err_quit("create thread 1 error: %s", strerror(err));

	err = pthread_join(tid1, (void **)&fp);
	if (err != 0)
		err_quit("wait for thread 1 terminate error: %s", strerror(err));

	printf("Create the second thread ...... ......\n");
	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0)
		err_quit("create thread 2 error: %s", strerror(err));
	sleep(1);	/* 休眠以让线程2得到执行机会 */

	printfoo("parent foo:", fp);
	exit(0);
}
