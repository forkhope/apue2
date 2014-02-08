#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: PENDIN, c_lflag, Linux
 * If set, any input that has not been read is reprinted by the system when
 * the next character is input. This action is similar to what happens when
 * we type the REPRINT characters.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认没有设置该标志位 */
    printf("term.c_lflag: %#x, PENDIN: %#x, &= %#x\n",
            term.c_lflag, PENDIN, term.c_lflag & PENDIN);

    term.c_lflag |= PENDIN;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable PENDIN: term.c_lflag: %#x, PENDIN: %#x, &= %#x\n",
            term.c_lflag, PENDIN, term.c_lflag & PENDIN);

    /* 开启 PENDIN 标志位后,执行cat命令,目前没有看出它怎么起作用的 */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
