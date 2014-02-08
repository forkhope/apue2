#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: HUPCL, c_cflag, POSIX.1
 * If set, the modem control lines are lowered (i.e., the modem connection
 * is broken) when the last process closes the device.
 *
 * 目前不清楚该标志位的具体作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_cflag: %#x, HUPCL: %#x, &= %#x\n",
            term.c_cflag, HUPCL, term.c_cflag & HUPCL);

    return 0;
}
