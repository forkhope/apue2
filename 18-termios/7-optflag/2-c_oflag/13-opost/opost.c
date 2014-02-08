#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: OPOST, c_oflag, POSIX.1
 * If set, implementation-defined output processing takes place. Refer
 * to Figure 18.6 for the various implementation-defined flags for the
 * c_oflag work.  Figure 18.6 中列表的 c_oflag 标志位有:
 * BSDLY, CMSPAR, CRDLY, FFDLY, NLDLY, OCRNL, OFDEL, OFILL, OLCUC,
 * ONLCR, ONLRET, ONOCR, ONOEOT, OPOST, OXTABS, TABDLY, VTDLY.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认开启了该标志位 */
    printf("term.c_oflag: %#x, OPOST: %#x, &= %#x\n",
            term.c_oflag, OPOST, term.c_oflag & OPOST);

    return 0;
}
