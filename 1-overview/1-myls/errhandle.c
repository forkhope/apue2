#include "apue.h"
#include <errno.h>		/* for definition of errno */
#include <stdarg.h>		/* ISC C variable */

static void err_doit(int, int, const char *, va_list);

/* exit()函数和return语句的区别是:exit()结束当前程序,在整个程序中,只要调用
 * exit(),整个程序会退出,即使是在非main()函数中调用也是一样;return语句是当
 * 前函数返回,当然如果是在主函数main(),自然也就结束当前进程了,如果不是,那就
 * 退回上一层调用它的函数.
 */

/* Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
	/* exit()函数将会终止进程,而不是从当前函数返回到上一个函数 */
	exit(1);
}

/* Fatal error unrelated to a system call.
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt, ap);
	va_end(ap);

	exit(1);
}

/* Print a message and return to caller.
 * Caller specifies the "errnoflag".
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];		/* MAXLINE 在 aput.h 中声明 */

	vsnprintf(buf, MAXLINE, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",
				strerror(errno)); /* strerror()返回errno对应的描述字符串 */

	strcat(buf, "\n");	/* 这里没有考虑到数组越界的情况 */
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flush all stdio output streams */
}
