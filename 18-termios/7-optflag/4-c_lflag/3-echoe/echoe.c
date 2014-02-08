#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHOE, c_lflag, POSIX.1
 * If set and If ICANON is set, the ERASE character erases the last
 * character in the current line from the display. This is usually done
 * in the terminal driver by writing the three-character sequence
 * backspace, space, backspace.
 *
 * If the WERASE character is supported, ECHOE causes the previous word to
 * be erased using one or more of the same three-character sequence.
 *
 * If the ECHOPRT flag is supported, the actions described here for ECHOE
 * assume that the ECHOPRT flag is not set.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printf("term.c_lflag: %#x, ECHOE: %#x, &= %#x\n",
            term.c_lflag, ECHOE, term.c_lflag & ECHOE);

    term.c_lflag &= ~ECHOE;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_lflag: %#x, ECHOE: %#x, &= %#x\n",
            term.c_lflag, ECHOE, term.c_lflag & ECHOE);

    /* 关闭 ECHOE 标志位后,执行cat命令,输入一串字符后,再按下backspace键
     * 或者Ctrl-Shift-?键时,将会显示 "^?" 两个字符,而不是会擦除前一个字符,
     * 如: "tian^?", 此时输入回车后,cat回显 "tia". 也就是确实删除了一个字
     * 符,只是在按下backspace后,终端上没有立刻回显出擦除的效果.
     * 此时,输入Ctrl-W,还是会立刻回显擦除单词的效果,感觉这个字符没有被
     * 随着禁掉.
     *
     * 奇怪的是,在终端上,执行stty -echoe屏蔽该标志位,在命令行里面,按下
     * backspace键还是会立刻回显出擦除效果,但是执行cat命令时,又不会立刻
     * 回显backsapce键的擦除效果. 目前原因不明,但是终端命令行的属性和stty
     * -a里面显示的属性不是完全一样的.(注意过,echoprt标志位也是关闭的)
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
