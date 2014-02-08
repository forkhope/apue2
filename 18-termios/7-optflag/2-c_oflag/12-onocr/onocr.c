#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ONOCR, c_oflag, XSI, Linux
 * If set, a CR is not output at column 0.
 * 
 * 目前没有看出该标志位具体是怎么起作用的.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_oflag: %#x, ONOCR: %#x, &= %#x\n",
            term.c_oflag, ONOCR, term.c_oflag & ONOCR);

    term.c_oflag |= ONOCR;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable ONOCR: term.c_oflag: %#x, ONOCR: %#x, &= %#x\n",
            term.c_oflag, ONOCR, term.c_oflag & ONOCR);

    /* 开启 ONOCR 标志位后,执行cat命令,没有看出什么特别 */
    system("cat");
    
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
