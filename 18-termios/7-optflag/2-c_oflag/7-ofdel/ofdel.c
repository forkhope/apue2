#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: OFDEL, c_oflag, XSI, Linux
 * If set, the output fill character is ASCII DEL; otherwise, it's
 * ASCII NUL, See the OFILL flag.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_oflag: %#x, OFDEL: %#x, &= %#x\n",
            term.c_oflag, OFDEL, term.c_oflag & OFDEL);

    return 0;
}
