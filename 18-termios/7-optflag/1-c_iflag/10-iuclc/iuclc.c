#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IUCLC, c_iflag, Linux
 * Map uppercase to lowercase on input.
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
    printf("term.c_iflag: %#x, IUCLC: %#x, &= %#x\n",
            term.c_iflag, IUCLC, term.c_iflag & IUCLC);

    term.c_iflag |= IUCLC;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable IUCLC: term.c_iflag: %#x, IUCLC: %#x, &= %#x\n",
            term.c_iflag, IUCLC, term.c_iflag & IUCLC);

    /* 开启 IUCLC 标志位后,执行cat命令,然后按下Caps Lock键,打开
     * 大写锁定,发现此时终端显示的输入字符就是小写的,输入回车后,
     * 回显出来的字符也是小写的.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
