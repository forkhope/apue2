#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: CSTOPB, c_cflag, POSIX.1
 * If set, two stop bits are used; otherwise, one stop bit is used.
 *
 * 停止位: 用于表示单个包的最后一位.典型值为1, 1.5, 和2位.由于数据是在传输
 * 线上定时的,并且每一个设备有其自己的时钟,很可能在通信中两台设备会出现不
 * 同步的情况,因此停止位不仅仅是表示传输的结束,并且提供计算机校正时钟同步的
 * 机会.适用于停止位的位数越多,不同时钟同步的容忍程序越大,但是数据传输率
 * 同时也越慢.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_cflag: %#x, CSTOPB: %#x, &= %#x\n",
            term.c_cflag, CSTOPB, term.c_cflag & CSTOPB);

    return 0;
}
