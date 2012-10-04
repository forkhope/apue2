#include <unistd.h>		/* sysconf() */
#include <limits.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

static void my_exit1(void);
static void my_exit2(void);
static void my_exit3(void);

/* With ISO C, a process can register up to 32 functions that are
 * automatically called by exit(). These are called exit handlers and are
 * registered by calling the atexit() function.
 * #include <stdlib.h>
 * int atexit(void (*func)(void));
 * 		Returns: 0 if OK, nonzero on error.
 * The exit() function calls there functions in reverse order of their
 * registration. Each function is called as many times as it was registered
 *
 * With ISO C and POSIX.1, exit() first calls the exit handlers and then
 * closes (via fclose()) all open streams.
 *
 * Note that the only way a program is executed by the kernel is when one 
 * of the exec() functions is called. The only way a process voluntarily
 * terminates is when _exit() or _Exit() is called, either explicitly or
 * implicitly (by calling exit()). A process can also be involuntarily
 * terminated by a signal.
 *
 * 上面提到了可以设置 32 个 exit handlers,这里书中有所描述:
 * ISO C requires that systems support at least 32 exit handlers. The
 * sysconf() function can be used to determine the maximum number of exit 
 * handlers supported by a given platform.
 */
int main(void)
{
	int i;
	
	if ((i = sysconf(_SC_ATEXIT_MAX)) == -1)
		printf("使用sysconf不能查询到_SC_ATEXIT_MAX对应的值\n");
	printf("%d\n", i);

	/* exit()函数会按照atexit()函数注册的反顺序来依次执行注册的函数,这些函数
	 * 可以是同一个函数.根据下面的注册顺序,会先执行 my_exit3()函数,接着执行
	 * 两次 my_exit1() 函数,最后才执行 my_exit2() 函数.
	 */
	if (atexit(my_exit2) != 0)
		printf("can't register my_exit2: %s\n", strerror(errno));
	if (atexit(my_exit1) != 0)
		printf("can't register my_exit1: %s\n", strerror(errno));
	if (atexit(my_exit1) != 0)
		printf("can't register my_exit1: %s\n", strerror(errno));
	if (atexit(my_exit3) != 0)
		printf("can't register my_exit3: %s\n", strerror(errno));

	return 0;
}

static void my_exit1(void)
{
	printf("first exit handler\n");
}

static void my_exit2(void)
{
	printf("second exit handler\n");
}

static void my_exit3(void)
{
	printf("third exit handler\n");
}
