#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: CLOCAL: c_cflag, POSIX.1
 * If set, the modem status lines are ignored. This usually means that the
 * device is directly attached. When this flag is not set, an open() of a
 * terminal device usually blocks until the modem answers a call and
 * establishes a connection, for example.
 *
 * 该标志位一般在串口编程中用到.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_cflag: %#x, CLOCAL: %#x, &= %#x\n",
            term.c_cflag, CLOCAL, term.c_cflag & CLOCAL);

    return 0;
}
