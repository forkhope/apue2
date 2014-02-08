#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHOPRT, c_lflag, Linux
 * If set and if both ICANON and ECHO are set, then the ERASE character
 * (and WERASE character, if supported) cause all the characters being
 * erased to be printed as they are erased. This is often useful on a
 * hard-copy terminal to see exactly which characters are being deleted.
 * 开启该标志位后,从执行结果来看,它会回显被显示的字符.如后面所述.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从输出结果看,终端默认没有开启该标志位 */
    printf("term.c_lflag: %#x, ECHOPRT: %#x, &= %d\n",
            term.c_lflag, ECHOPRT, term.c_lflag & ECHOPRT);

    term.c_lflag |= ECHOPRT;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    /* 开启ECHOPRT标志位后,执行cat命令,输入"tian xia you",然后输入
     * WERASE字符(即Ctrl-W),会显示"tian xia you\uoy",即在行末添加了
     * "\uoy",从字符串末尾往前回显了被擦除的字符,此时输入回车后,会
     * 输出"tian xia".接着输入"tian xia",再按下Backspace键三次,会
     * 显示"tian xia\aix",每按一次Backspace,就会显示一个字符,先显示
     * a,然后是i,最后是x.此时按下回车,会输出"tian".
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
