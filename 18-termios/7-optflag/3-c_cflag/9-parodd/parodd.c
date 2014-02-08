#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: c_cflag, POSIX.1
 * If set, the parity for outgoing and incoming characters is odd parity.
 * Otherwise, the parity is even parity. Note that the PARENB flag controls
 * the generation and detection of parity.
 *
 * The PARODD flag also controls whether mark or space parity is used when
 * either the CMSPAR or PAREXT flag is set.
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
    printf("term.c_cflag: %#x, PARODD: %#x, &= %#x\n",
            term.c_cflag, PARODD, term.c_cflag & PARODD);

    return 0;
}
