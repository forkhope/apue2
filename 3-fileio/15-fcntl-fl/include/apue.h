#ifndef APUE_H
#define APUE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE	4096	/* max line length */

void err_sys(const char *fmt, ...);
void err_quit(const char *fmt, ...);

#endif	/* APUE_H */
