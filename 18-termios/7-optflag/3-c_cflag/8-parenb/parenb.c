#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: c_cflag, POSIX.1
 * If set, parity generation is enabled for outgoing characters, and parity
 * checking is performed on incoming characters. The parity is odd if
 * PARODD is set; otherwise, it is even parity. See also the discussion of
 * the INPCK, IGNPAR, and PARMRK flags.
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
    printf("term.c_cflag: %#x, PARENB: %#x, &= %#x\n",
            term.c_cflag, PARENB, term.c_cflag & PARENB);

    return 0;
}
