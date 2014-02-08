#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ONLCR: c_oflag, XSI, Linux
 * If set, map NL to CR-NL on output.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认开启了该标志位.
     * CR是回车的意思,表示回到行首. NL是换行的意思,表示到下一行.
     * 将NL映射为CR-NL,就是先回到行首,再移到下一行.所以当我们
     * 输入NL后,光标会移动到下一行行首.
     */
    printf("term.c_oflag: %#x, ONLCR: %#x, &= %#x\n",
            term.c_oflag, ONLCR, term.c_oflag & ONLCR);

    term.c_oflag &= ~ONLCR;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error") ;
        return 1;
    }

    printf("###\n");
    /* 关闭 ONLCR 标志位后,输入NL或者CR(终端默认开启ICRNL标志位),
     * 光标会移动到下一行,但不是在下一行行首,而是在之前光标所在
     * 位置的后一位.例如:
     * ###
     *    tian              // 这里是输入的字符串
     *        tian          // 这里是回显的字符串
     *            xia       // 这里是输入的字符串
     *               xia    // 这里是回显的字符串
     */
    system("cat");
    printf("disable ONLCR: term.c_oflag: %#x, ONLCR: %#x, &= %#x\n",
            term.c_oflag, ONLCR, term.c_oflag & ONLCR);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error") ;
        return 1;
    }
    return 0;
}
