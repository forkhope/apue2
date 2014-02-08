#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHOKE, c_lflag, not in POSIX.1
 * If set and if ICANON is set, the KILL character is echoed by
 * erasing each character on the line. The way in which each character
 * is erased is selected by the ECHOE and ECHOPRT flags.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printf("term.c_lflag: %#x, ECHOKE: %#x, &= %#x\n",
            term.c_lflag, ECHOKE, term.c_lflag & ECHOKE);

    term.c_lflag &= ~ECHOKE;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_lflag: %#x, ECHOKE: %#x, &= %#x\n",
            term.c_lflag, ECHOKE, term.c_lflag & ECHOKE);

    /* 禁用ECHOKE后,输入KILL字符,会看到光标换了一样.因为此时ECHOK
     * 是开着的.如果同时禁掉ECHOK,就不会看到光标换行了,可以在终端
     * 里面执行"stty -echok -echoke; cat"命令来测试这一点.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
