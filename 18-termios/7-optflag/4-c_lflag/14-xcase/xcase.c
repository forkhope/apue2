#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: XCASE, c_lflag, Linux
 * If set and if ICANON is also set, the terminal is assumed to be uppercase
 * only, and all input is converted to lowercase. To input an uppercase
 * character, precede it with a backslash. Similarly, an uppercase character
 * is output by the system by being preceded by a backslash. (This option
 * flag is obsolete today, since mose, if not all, uppercase-only terminals
 * have disappered.)
 *
 * 书中说Linux支持该标志位,但是man tcgetattr手册中对该标志位描述为:
 * XCASE: (not in POSIX; not supported under Linux).
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
    printf("term.c_lflag: %#x, XCASE: %#x, &= %#x\n",
            term.c_lflag, XCASE, term.c_lflag & XCASE);

    term.c_lflag |= XCASE;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable XCASE: term.c_lflag: %#x, XCASE: %#x, &= %#x\n",
            term.c_lflag, XCASE, term.c_lflag & XCASE);

    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
