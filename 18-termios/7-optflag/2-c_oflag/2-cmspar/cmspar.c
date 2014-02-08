#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: CMSPAR: c_oflag, Linux, not in POSIX.
 * man tcgetattr手册中对该标志位描述为:
 * Use "stick" (mark/space) parity (supported on certain serial devices):
 * if PARODD is set, the parity bit is always 1; if PARODD is not set, then
 * the parity bit is always 0). [requires _BSD_SOURCE or _SVID_SOURCE]
 *
 * 目前不清楚这个标志位具体的作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_oflag: %#x, CMSPAR: %#x, &= %#x\n",
            term.c_oflag, CMSPAR, term.c_oflag & CMSPAR);

    return 0;
}
