#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IXON, c_iflag, POSIX.1
 * If set, start-stop output control is enabled. When the terminal driver
 * receives a STOP character, output stops. While the output is stopped,
 * the next START character resumes the output. If this flag is not set,
 * the START and STOP characters are read by the process as normal
 * characters. 即, START字符(Ctrl-Q) 和 STOP字符(Ctrl-S) 必须开启了IXON
 * 才有效,如果没有开启IXON,这两个字符将被当作普通字符处理.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认开启了 IXON 标志位 */
    printf("term.c_iflag: %#x, IXON: %#x, &= %#x\n",
            term.c_iflag, IXON, term.c_iflag & IXON);

    /* 由于终端开启了IXON标志位,我们可以输入STOP字符(Ctrl-S)来
     * 停止yes命令的输出,再输入START字符(Ctrl-Q)来恢复它的输出.
     */
    system("yes");

    term.c_iflag &= ~IXON;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("disable IXON: term.c_iflag: %#x, IXON: %#x, &= %#x\n",
            term.c_iflag, IXON, term.c_iflag & IXON);

    /* 关闭 IXON 标志位后,STOP字符和START字符并失去它们的特殊含义,
     * 此时输入STOP无法停止yes命令的输出,输入START字符也就不能恢复
     * 它的输出了.
     */
    system("yes");

    /* 执行cat命令,此时输入Ctrl-S将会回显^S,输入Ctrl-Q回显^Q,它们
     * 被当成普通字符来处理,cat命令读取到这两个字符,并显示出来.
     */
    printf("########## execute the 'cat' #######\n");
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
