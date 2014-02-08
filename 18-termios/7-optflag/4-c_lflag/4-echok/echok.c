#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHOK, c_lflag, POSIX.1
 * If set and if ICANON is set, the KILL character erases the current
 * line from the display or outputs the NL character (to emphasize that
 * the entire line was erased).
 * If the ECHOKE flag is supported, this description of ECHOK assumes
 * that ECHOKE is not set.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printf("term.c_lflag: %#x, ECHOK: %#x, &= %#x\n",
            term.c_lflag, ECHOK, term.c_lflag & ECHOK);

    term.c_lflag &= ~ECHOK;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_lflag: %#x, ECHOK: %#x, &= %#x\n",
            term.c_lflag, ECHOK, term.c_lflag & ECHOK);

    /* 关闭ECHOK标志位后,执行cat命令,此时输入Ctrl-U,将不会立刻
     * 回显出整行被擦除的效果,而是显示^U,例如:tian^U.
     * 然后输入回车后,cat打印出一个空行,即之前输入的tian被擦除了.
     * 
     * 在上面的描述中提到,设置了ECHOK后,KILL字符会输出一个NL字符,
     * 以强调整行被删除了,但是下面执行cat命令时却没有看到这个效果,
     * 原来书中又提到,此时ECHOKE应该没有被设置.查看了一下,此时,
     * ECHOKE确实是被设置了,禁用ECHOKE后,输入KILL字符就会看到光标
     * 换了一行.可以在终端里面执行"stty -echoke; cat"来测试这一点.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
