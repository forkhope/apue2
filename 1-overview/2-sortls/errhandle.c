#include "apue.h"
#include <errno.h>		/* for definition of errno */
#include <stdarg.h>		/* ISO C variable */

static void err_doit(int, int, const char *, va_list);

/* Fatal error related to a system call.
 * Print a message and terminate.
 */
void err_quit(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, 0, fmt,ap);
	va_end(ap);

	exit(1);
}

/* Fatal error unrelated to system call.
 * Print a message and terminate.
 */
void err_sys(const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	err_doit(1, errno, fmt, ap);
	va_end(ap);

	exit(1);
}

/* Print a message and return to caller.
 * Caller specifies "errnoflag".
 */
static void err_doit(int errnoflag, int error, const char *fmt, va_list ap)
{
	char buf[MAXLINE];

	vsnprintf(buf, MAXLINE, fmt, ap);
	if (errnoflag)
		snprintf(buf+strlen(buf), MAXLINE-strlen(buf), ": %s",
				strerror(errno));
	strcat(buf, "\n");
	fflush(stdout);		/* in case stdout and stderr are the same */
	fputs(buf, stderr);
	fflush(NULL);		/* flush all stdin output streams */
}
