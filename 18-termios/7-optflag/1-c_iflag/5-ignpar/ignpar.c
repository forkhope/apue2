#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IGNPAR, c_iflag, POSIX.1
 * When set, an input byte with a framing error (other than a BREAK) or
 * an input byte with a parity error is ignored.
 *
 * 惯例说一句,目前不清楚该标志位的具体作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_iflag: %#x, IGNPAR: %#x, &= %#x\n",
            term.c_iflag, IGNPAR, term.c_iflag & IGNPAR);

    return 0;
}
