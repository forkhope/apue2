#include "apue.h"
#include <errno.h>
#include <stdarg.h>

static void err_doit(int, int, const char *, va_list);

/* Nonfatal error related to a system call.
 * Print a message and return.
 */
void err_ret(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);
}

/* Print a message and return to caller.
 * Caller specifies "errnoflag"
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];

	vsnprintf(buf, MAXLINE, fmt, ap);
	if (errnoflag)
		snprintf(buf-strlen(buf), MAXLINE-strlen(buf), ": %s",
				strerror(error));
	strcat(buf, "\n");
	fflush(stdout);			/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);			/* flushes all stdin output streams */
}
