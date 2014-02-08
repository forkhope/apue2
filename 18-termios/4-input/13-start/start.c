#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <stdlib.h>

/* 终端I/O特殊字符 ---- START
 * The start character. This character is recognized on input if the IXON
 * flag is set and is automatically generated as output if the IXOFF flag
 * is set. A received START character with IXON set causes stopped output
 * (from a previously entered STOP character) to restart. In this case,
 * the START character is discarded when processed (i.e., it is not passed
 * to the process, 括号里这句话的意思是说,该字符会被处理完后被删除,不送向
 * 读进程.即用户输入了这个字符,终端驱动对这个字符做了特殊处理,用户读取时,
 * 不会读到该字符).
 * When IXOFF is set, the terminal driver automatically generates a START
 * character to resume input that it had previously stopped, when the new
 * input will not overflow the input buffer.
 *
 * IXON标志表示: enable start/stop output flow
 * IXOFF标志表示: enable start/stop input flow control
 *
 * 由上面可知,IXON被设置时,需要先输入STOP字符停止输出,再输入START字符才能
 * 看到该字符的效果,它会恢复被停止的输出.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来c_cc[VSTART]的十进制值是17,八进制值是021.根据man tcgetattr
     * 可知,该字符对应的键位是Ctrl-Q.而STOP字符的键位是Ctrl-S.
     */
    printf("term.c_cc[VSTART]: %#o, %d\n",
            term.c_cc[VSTART], term.c_cc[VSTART]);

    /* 执行top命令,该命令会动态更新一些内容,输入STOP字符(Ctrl-S)后,发现
     * 它将不再更新内容,直到输入START字符(Ctrl-Q)后,才重新更新内容.
     */
    system("top");

    /* 由输出结果可知,终端默认设置了IXON, 关闭了IXOFF */
    printf("term.c_iflag: %#x, IXON: %#x, &=: %#x\n", term.c_iflag,
            IXON, term.c_iflag & IXON);
    printf("term.c_iflag: %#x, IXOFF: %#x, &=: %#x\n", term.c_iflag,
            IXOFF, term.c_iflag & IXOFF);

    /* 下面设置了IXOFF标志, 关闭IXON标志 */
    term.c_iflag |= IXOFF;
    term.c_iflag &= ~IXON;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("term.c_iflag: %#x, IXON: %#x, &=: %#x\n", term.c_iflag,
            IXON, term.c_iflag & IXON);
    printf("term.c_iflag: %#x, IXOFF: %#x, &=: %#x\n", term.c_iflag,
            IXOFF, term.c_iflag & IXOFF);

    /* 此时再执行top命令,发现STOP字符(Ctrl-S)不能再停止输出,START字符
     * (Ctrl-Q)当然也就不能恢复输出.此时,再输入这两个字符,top甚至会
     * 提示出错: Unknown command - try 'h' for help.
     * 目前还不明白这两个字符在IXOFF标志下怎么起作用.
     */
    system("top");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    return 0;
}
