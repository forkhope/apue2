#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IMAXBEL, c_iflag, Linux
 * Ring bell when input queue is full.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认设置了该标志位 */
    printf("term.c_iflag: %#x, IMAXBEL: %#x, &= %#x\n",
            term.c_iflag, IMAXBEL, term.c_iflag & IMAXBEL);

    return 0;
}
