#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- SUSP
 * The suspend job-control character. This character is recognized on input
 * if job control is supported and if the ISIG flag is set. The suspend
 * character generates the SIGTSTP signal, which is sent to all processes
 * in the foreground process group. This character is discarded when
 * processed (i.e., it is not passed to the processes).
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来term.c_cc[VSUSP]的十进制值是26,八进制值是032,根据
     * man tcgetattr可知,该字符对应的键位是Ctrl-Z.
     */
    printf("term.c_cc[VSUSP]: %#o, %d\n", term.c_cc[VSUSP],
            term.c_cc[VSUSP]);

    /* 由下面的输出结果可知,终端默认开启了ISIG标志位 */
    printf("term.c_lflag: %#x, ISIG: %#x, &= %#x\n",
            term.c_lflag, ISIG, term.c_lflag & ISIG);
    system("cat");

    /* 将 SUSP 字符对应的键位设成 'z' */
    term.c_cc[VSUSP] = 'z';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("term.c_cc[VSUSP]: %d\n", term.c_cc[VSUSP]);
    system("cat");

    /* 关闭终端的ISIG标志位,此时 SUSP 字符将丢失其特殊含义 */
    term.c_lflag &= ~ISIG;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("----- after disable the ISIG flag ----\n");
    system("cat");

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
