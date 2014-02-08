#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- STOP
 * The stop character. This character is recognized on input if the IXON
 * flag is set and is automatically generated as output if the IXOFF flag
 * is set. A received STOP character with IXON set stops the output. In
 * this case, the STOP character is discarded when processed (i.e., it is
 * not passed to the process). The stopped output is restarted when a
 * START character is entered.
 *
 * When IXOFF is set, the terminal driver automatically generates a STOP
 * character to prevent the input buffer from overflowing. 目前还不理解
 * 这段话具体含义是什么.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来term.c_cc[VSTOP]的十进制值是19,八进制是023,根据
     * man tcgetattr 可知,该字符对应的键位是Ctrl-S.
     */
    printf("term.c_cc[VSTOP]: %#o, %d\n", term.c_cc[VSTOP],
            term.c_cc[VSTOP]);

    /* 由下面输出结果可知,终端默认开启了 IXON 标志 */
    printf("term.c_iflag: %#x, IXON: %#x, &= %#x\n",
            term.c_iflag, IXON, term.c_iflag & IXON);
    system("top");

    term.c_iflag &= ~IXON;  /* 屏蔽 IXON 标志位 */
    term.c_iflag |= IXOFF;  /* 打开 IXOFF 标志位  */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    /* 查看 IXOFF 标志位是否开启了 */
    printf("term.c_iflag: %#x, IXOFF: %#x, &= %#x\n",
            term.c_iflag, IXOFF, term.c_iflag & IXOFF);
    system("top");

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
