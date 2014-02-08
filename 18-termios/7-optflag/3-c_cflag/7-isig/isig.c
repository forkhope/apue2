#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ISIG, c_lflag, POSIX.1
 * If set, the input characters are compared against the special characters
 * that cause the terminal-generated signals to be generated (INTR, QUIT,
 * SUSP, and DSUSP); if equal, the corresponding signal is generated.
 *
 * INRT: 对应Ctrl-C
 * QUIT: 对应Ctrl-\
 * SUSP: 对应Ctrl-Z
 * DSUSP: 对应Ctrl-Y. Linux下,输入Ctrl-Y不会触发DSUSP信号,不支持该功能.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认设置该标志位 */
    printf("term.c_lflag: %#x, ISIG: %#x, &= %#x\n",
            term.c_lflag, ISIG, term.c_lflag & ISIG);

    term.c_lflag &= ~ISIG;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("disable ISIG: term.c_lflag: %#x, ISIG: %#x, &= %#x\n",
            term.c_lflag, ISIG, term.c_lflag & ISIG);

    /* 关闭 ISIG 标志位后,输入Ctrl-C将不能终止下面的cat命令,输入Ctrl-Z
     * 也不能暂停该命令了.但是此时输入Ctrl-D还是能终止输入,因为Ctrl-D
     * 并不是产生一个信号,它的功能没有被禁掉.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
