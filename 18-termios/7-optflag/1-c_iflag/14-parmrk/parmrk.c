#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: c_iflag, POSIX.1
 * When set and if IGNPAR is not set, a byte with a framing error (other
 * than a BREAK) or a byte with a parity error is read by the process as
 * the three-character sequence \377, \0, X, where X is the byte received
 * in error. If ISTRIP is not set, a valid \377 is passed to the process
 * as \377, \377. If neither IGNPAR nor PARMRK is set, a byte with a
 * framing error (other than a BREAK) or with a parity error is read as a
 * single character \0.
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

    /* 从执行结果看,终端默认没有设置该标志位 */
    printf("term.c_iflag: %#x, PARMRK: %#x, &= %#x\n",
            term.c_iflag, PARMRK, term.c_iflag & PARMRK);

    return 0;
}
