#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <signal.h>

/* 终端I/O特殊字符 ---- INTR
 * INTR: The interrupt character. This character is recognized on input
 * if the ISIG flag is set and generates the SIGINT signal that is sent
 * to all processes in the foreground process group. This character is
 * discarded when processed (i.e., it is not passed to the process).
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来的term.c_cc[VINTR]的值是3,对应Ctrl-C字符. */
    printf("term.c_cc[VINTR]: %d, %#o\n", term.c_cc[VINTR],
            term.c_cc[VINTR]);

    /* 根据上面的描述, INTR字符需要在设置了ISIG标志位后才有效,下面
     * 判断是否设置了该标志位.根据运行结果,确实是设置了该标志位.
     */
    printf("term.c_lflag: %#x, ISIG: %#x\n", term.c_lflag, ISIG);
    if (term.c_lflag & ISIG) {
        printf("term.c_lflag has set the ISIG flag\n");
    }
    else {
        printf("term.c_lflag has not set the ISIG flag\n");
    }

    /* 将中断字符设置为']'字符,此后,输入']'字符将会给终端的前台进程
     * 发送SIGINT信号.例如输入cat命令,然后输入']',将会终止cat命令.
     */
    term.c_cc[VINTR] = ']';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("term.c_cc[VINTR]: %d, %#o\n", term.c_cc[VINTR],
            term.c_cc[VINTR]);

    /* 执行 stty -a 命令查看终端属性,可以看到里面有个 isig 的属性,
     * 表示 isig 属性已经被设置.
     */
    system("stty -a | grep isig");
    printf("----------------------------------------\n");

    /* 从 c_lflag 中去掉 ISIG 标志位,此时,INTR字符将不再有效.运行结果
     * 也确实如此,当代码执行到这里之后,再输入']'字符将不会再给前台进程
     * 发送SIGINT信号,例如执行cat命令,输入']'后,cat命令还是会继续执行.
     */
    term.c_lflag &= ~ISIG;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("again: term.c_lflag: %#x, ISIG: %#x\n", term.c_lflag, ISIG);
    if (term.c_lflag & ISIG) {
        printf("again: term.c_lflag has set the ISIG flag\n");
    }
    else {
        printf("again: term.c_lflag has not set the ISIG flag\n");
    }

    /* 执行 stty -a 命令查看终端属性,可以看到里面有个 -isig 的属性,
     * 表示 isig 属性被去掉了.
     */
    system("stty -a | grep isig");

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr oldterm STDIN_FILENO error");
        return 1;
    }

    return 0;
}
