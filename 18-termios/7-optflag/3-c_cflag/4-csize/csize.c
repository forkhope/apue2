#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: CSIZE: c_cflag, POSIX.1
 * This field is a mask that specifies the number of bits per byte for both
 * transmission and reception. This size does not include the parity bit,
 * if any. The values for the field defined by this mask are CS5, CS6, CS7,
 * CS8, for 5, 6, 7, and 8 bits per byte, repectively.
 *
 * 下面是找到的一段关于 "数据位" 的描述:
 * 在计算机发送的数据包中,实际的数据往往不会是8位.在串口通信中,可以选择5,6,
 * 7或8位.设定数据位主要考虑所要传输的数据内容.如果要传输的是标准的ASCII码,
 * 由于ASCII码的范围是0~127,因此使用7位就可以了.如果要传输的是扩展ASCII码,
 * 其范围是0~255,必须使用8位.当然,7位或8位数据为中不仅仅是数据,还包括开始/
 * 停止位,数据位以及奇偶校验位等.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }


    printf("term.c_cflag: %#x, CSIZE: %#x, &= %#x\n",
            term.c_cflag, CSIZE, term.c_cflag & CSIZE);
    printf("CS5: %#4x, CS6: %#4x\nCS7: %#4x, CS8: %#4x\n",
            CS5, CS6, CS7, CS8);

    return 0;
}
