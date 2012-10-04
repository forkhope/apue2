#ifndef APUE_H
#define APUE_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXLINE	4096	/* max line length */

void err_sys(const char *, ...);

int dup2_l(int oldfd, int newfd);

#endif	/* APUE_H */
