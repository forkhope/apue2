#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: OFILL, c_oflag, XSI, Linux
 * If set, fill characters (either ASCII DEL or ASCII NUL; see the
 * OFDEL flag) are transmitted for a delay, instead of using a timed
 * delay. See the six delay masks: BSDLY, CRDLY, FFDLY, NLDLY, TABDLY,
 * and VTDLY.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_oflag: %#x, OFILL: %#x, &= %#x\n",
            term.c_oflag, OFILL, term.c_oflag & OFILL);

    return 0;
}
