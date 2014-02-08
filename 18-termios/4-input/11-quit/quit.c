#include <stdio.h>
#include <stdlib.h>     /* system() */
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- QUIT
 * The quit character. This character is recognized on input if the ISIG
 * flag is set. The quit character generates the SIGQUIT signal, which is
 * sent to all processes in the foreground process group. This character
 * is discarded when processed (i.e., it is not passed to the process).
 * Recall from Figure 10.1 that the difference between INTR and QUIT is
 * that the QUIT character not only terminates the process by default, but
 * also generates a core file.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来c_cc[VQUIT]的十进制值是28,八进制值为034.根据man tcgetattr
     * 手册,可以其对应的键位是Ctrl-\.
     */
    printf("term.c_cc[VQUIT]: %d, %#o\n", term.c_cc[VQUIT],
            term.c_cc[VQUIT]);

    /* 根据上面的描述,VQUIT需要设置了ISIG标志位才有效,下面查看终端
     * 是否设置了该标志位.
     */
    printf("term.c_lflag: %#x, ISIG: %#x, has set? ---- %s\n",
            term.c_lflag, ISIG,
            (term.c_lflag & ISIG) ? "YES" : "NO");

    term.c_cc[VQUIT] = 'a';     /* 将VQUIT对应的字符设置为 'a' */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("after set VQUIT to 'a': term.c_cc[VQUIT]: %d, %#o\n",
            term.c_cc[VQUIT], term.c_cc[VQUIT]);
    system("cat");

    term.c_lflag &= ~ISIG;      /* 去掉终端中的 ISIG 标志位 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("after clear: term.c_lflag: %#x, ISIG: %#x, has set? ---- %s\n",
            term.c_lflag, ISIG,
            (term.c_lflag & ISIG) ? "YES" : "NO");
    system("cat");

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
