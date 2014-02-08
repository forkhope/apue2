#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IXANY, c_iflag, XSI, Linux
 * Enable any characters to restart output.
 * man tcgetattr 手册中对 IXANY 的描述为:
 * Typing any character will restart stopped output. (The default
 * is to allow just the START character to restart output.)
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认设置了该标志位.
     * 此时,可以输入任意字符来恢复被暂停的输出.
     */
    printf("term.c_iflag: %#x, IXANY: %#x, &= %#x\n",
            term.c_iflag, IXANY, term.c_iflag & IXANY);

    /* system("top");
     * 为了不断的看到输出,执行yes命令是个不错的选择.不过
     * 执行yes命令后,屏幕刷新太快,就看不到上面printf()的
     * 输出了,此时上面的 system("top") 就派上用场了.
     */
    system("yes");

    term.c_iflag &= ~IXANY;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("disable IXANY: term.c_iflag: %#x, IXANY: %#x, &= %#x\n",
            term.c_iflag, IXANY, term.c_iflag & IXANY);

    /* system("top");
     * 关闭 IXANY 后,只能通过START字符(Ctrl-Q)来恢复被暂停的输出了.
     */
    system("yes");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
