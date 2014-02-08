#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>

char buf1[1024];
char buf2[1024];
int fd;

void *thr_rtn(void *arg)
{
	int n;

	if ((n = pread(fd, buf1, 1024, 3)) < 0) {
		printf("thread 1: pread error: %s\n", strerror(errno));
		return (void *)1;
	}
	printf("thread 1: pread: buf1: %s", buf1);

	return (void *)0;
}

void *thr_rtn2(void *arg)
{
	int n;

	if ((n = pread(fd, buf2, 1024, 10)) < 0) {
		printf("thread 2: pread error: %s\n", strerror(errno));
		return (void *)1;
	}
	printf("thread 2: pread: buf2: %s", buf2);

	return (void *)0;
}

/* 书中第12.10小节提到,多线程环境下要使用pread(),pwrite()函数,而不要用
 * read(),write()函数.因为pread()和pwrite()函数不会改变文件偏移指针,避免
 * 多线程调度时,文件偏移指针发生改变,导致读写的位置不如预期.
 * 编译的时候,要添加 -D_XOPEN_SOURCE=500选项,否则会报警:
 * warning: implicit declaration of function ‘pread’
 */
int main(void)
{
	pthread_t tid1, tid2;
	int err;

	if ((fd = open("today", O_RDWR)) < 0) {
		printf("open file 'today' error: %s\n", strerror(errno));
		return 1;
	}

	err = pthread_create(&tid1, NULL, thr_rtn, NULL);
	if (err != 0) {
		printf("pthread_create: thread 1: error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_create(&tid2, NULL, thr_rtn2, NULL);
	if (err != 0) {
		printf("pthread_create: thread 2: error: %s\n", strerror(err));
		return 1;
	}

	err = pthread_join(tid1, NULL);
	if (err != 0) {
		printf("pthread_join: thread 1: error: %s\n", strerror(err));
		return 1;
	}
	err = pthread_join(tid2, NULL);
	if (err != 0) {
		printf("pthread_join: thread 2: error: %s\n", strerror(err));
		return 1;
	}

	return 0;
}
