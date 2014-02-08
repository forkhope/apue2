#include <stdio.h>
#include <stdlib.h>     /* system() */
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- REPRINT, Not in POSIX.
 * The reprint character. This character is recognized on input in
 * extended, canonical mode (both IEXTEN and ICANON flags set) and
 * causes all unread input to be output (reechoed). This character
 * is discarded when processed (i.e., it is not passed to the process).
 * 它只是回显还没有被读取的字符,而不是复制一份还没有被读取的字符,
 * 回显之后,还没有被读取的字符还是保持原样,不会变成有两份输入字符.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来的c_cc[VREPRINT]的十进制值是18,八进制值是022,根据
     * man tcgetattr可知,对应的键位是Ctrl-R.
     */
    printf("term.c_cc[VREPRINT]: %#o, %d\n",
            term.c_cc[VREPRINT], term.c_cc[VREPRINT]);

    /* 下面执行cat命令,当输入一行字符串,但还没有按下回车之前,输入Ctrl-R,
     * 刚才的输入会换行,然后重新显示出来,但是按下回车后,cat回显出来的字符
     * 串只有一行.也就是说,输入REPRINT字符,只是让还没有被读取的输入字符
     * 回显出来,而不是再复制一份输入字符,没有被读取的输入字符还是那么多.
     * 一个cat命令的输入例子如下:
     * john:~/program/apue2/18-termios/4-input/12-reprint$cat
     * tian^R   // 在输入tian后,输入Ctrl-R,终端显示出来"^R"两个字符
     * tian     // 然后,终端换行回显刚才的输入.此时输入回车
     * tian     // 这里是cat命令读取到输入后,回显出来的输出.
     */
    system("cat");

    /* 将 REPRINT 对应的字符设置为 'w' 字符 */
    term.c_cc[VREPRINT] = 'w';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("after set term.c_cc[VREPRINT] to 'w': %#o, %d\n",
            term.c_cc[VREPRINT], term.c_cc[VREPRINT]);
    system("cat");

    /* 恢复终端原先的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    return 0;
}
