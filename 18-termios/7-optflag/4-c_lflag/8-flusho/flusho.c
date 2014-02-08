#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: FLUSHO, c_lflag, Linux
 * If set, output is being flushed. This flag is set when we type the
 * DISCARD character; the flag is cleared when we type another DISCARD
 * character. We can also set or clear this condition by setting or
 * clearing this terminal flag.
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
    printf("term.c_lflag: %#x, FLUSHO: %#x, &= %#x\n",
            term.c_lflag, FLUSHO, term.c_lflag & FLUSHO);

    term.c_lflag |= FLUSHO;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    /* 开启FLUSHO标志位后,执行FLUSHO.目前没有看出有什么不同 */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
