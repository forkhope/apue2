#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: BSDLY: c_oflag, XSI)
 * Backspace delay mask. The values for the mask are BS0 or BS1.
 * man tcgetattr手册中对该标志位描述为:
 * BSDLY: Backspace delay mask. Values are BS0 Or BS1. (Has never been
 *        implemented). [requires _BSD_SOURCE or _SVID_SOURCE or
 *        _XOPEN_SOURCE]
 * 目前不知道该标志位更详细的含义,根据man手册来看,Linux也没有真正实现它.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_oflag: %#x, BSDLY: %#x, &= %#x\n",
            term.c_oflag, BSDLY, term.c_oflag & BSDLY);

    switch (term.c_oflag & BSDLY) {
        case BS0:
            printf("backspace delay: BS0\n");
            break;
        case BS1:
            printf("backspace delay: BS1\n");
            break;
        default:
            printf("backspace delay: unknown\n");
            break;
    }

    return 0;
}
