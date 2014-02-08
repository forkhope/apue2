#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: OCRNL, c_oflag, XSI, Linux
 * If set, map CR to NL on output.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有设置该标志位 */
    printf("term.c_oflag: %#x, OCRNL: %#x, &= %#x\n",
            term.c_oflag, OCRNL, term.c_oflag & OCRNL);

    return 0;
}
