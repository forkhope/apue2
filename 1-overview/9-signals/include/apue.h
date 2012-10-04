#ifndef APUE_H
#define APUE_H

#include <stdio.h>
#include <string.h>		/* strerror() */
#include <stdlib.h>		/* exit() */
#include <unistd.h>		/* fork() */
#include <signal.h>		/* signal() */

#define MAXLINE 	4096	/* max line length */

void err_sys(const char *, ...);
void err_ret(const char *, ...);

#endif	/* APUE_H */
