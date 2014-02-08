#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ICANON, c_lflag, POSIX.1
 * If set, canonical mode is in effect. This enables the following
 * characters: EOF, EOL, EOL2, ERASE, KILL, REPRINT, STATUS, and WERASE.
 * The input characters are assembled into lines.
 *
 * If canonical mode is not enabled, read requests are satisfied directly
 * from the input queue. A read does not return until at least MIN bytes
 * have been received or the timeout value TIME has expired between bytes.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认就开启了 ICANON 标志位 */
    printf("term.c_lflag: %#x, ICANON: %#x, &= %#x\n",
            term.c_lflag, ICANON, term.c_lflag & ICANON);
    system("cat");

    term.c_lflag &= ~ICANON;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    
    printf("disable ICANON: term.c_lflag: %#x, ICANON: %#x, &= %#x\n",
            term.c_lflag, ICANON, term.c_lflag & ICANON);

    /* 在开启ICANON标志位时,需要输入回车或者换行后,才会回显输入的字符.
     * 而关闭ICANON标志位后,执行cat命令,每输入一个字符就会立刻回显该字符,
     * 此时,输入EOF (即Ctrl-D), WERASE (即Ctrl-W), KILL (即Ctrl-U)等,都
     * 不会起作用,而是会回显^D, ^W, ^U和一些乱码.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
