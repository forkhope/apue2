#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IEXTEN, c_lflag, POSIX.1
 * If set, the extended, implementation-defined special characters are
 * recognized and processed.
 * man tcgetattr 手册中对该标志位的描述为:
 * Enable implementation-defined input processing. This flag, as well as
 * ICANON must be endalbed for the special characters EOL2, LNEXT, REPRINT,
 * WERASE to be interpreted, and for the IUCLC flag be to effective.
 * 即关闭 IEXTEN 标志位后,上述字符就会受到影响.
 *
 * POSIX.1标准中对IEXTEN的部分描述为:
 * If IEXTEN is set, implementation-defined functions shall be recognized
 * from the input data. It is implementation-defined how IEXTEN being set
 * interacts with ICANON, ISIG, IXON, or IXOFF. If IEXTEN is not set,
 * implementation-defined functions shall not be recognized and the
 * corresponding input characters are processed as described for ICANON,
 * ISIG, IXON, or IXOFF.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认开启了 IEXTEN 标志位 */
    printf("term.c_lflag: %#x, IEXTEN: %#x, &= %#x\n",
            term.c_lflag, IEXTEN, term.c_lflag & IEXTEN);

    term.c_lflag &= ~IEXTEN;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("disable IEXTEN: term.c_lflag: %#x, IEXTEN: %#x, &= %#x\n",
            term.c_lflag, IEXTEN, term.c_lflag & IEXTEN);

    /* 关闭 IEXTEN 标志位后,执行cat命令,会发现WERASE (Ctrl-W), LNEXT
     * (Ctrl-V)都失去了他们的特殊含义.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
