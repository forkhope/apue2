#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: CTSCTS: c_cflag, not in POSIX.1
 * On the Linux, enables both inbound and outbound hardware flow control
 * (equivalent to CCTS_OFLOW | CRTS_IFLOW).
 * 从APUE2和man手册来看,Linux不支持 CCTS_IFLOW 和 CRTS_IFLOW 这两个标志位.
 * man tcgetattr中对该标志位描述为: Enable RTS/CTS (hardware) flow control.
 * [requires _BSD_SOURCE or _SVID_SOURCE].
 *
 * 目前不清楚该标志位具体的作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_cflag: %#x, CRTSCTS: %#x, &= %#x\n",
            term.c_cflag, CRTSCTS, term.c_cflag & CRTSCTS);

    return 0;
}
