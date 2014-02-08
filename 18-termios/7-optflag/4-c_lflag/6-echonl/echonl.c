#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHONL, c_lflag, POSIX.1
 * If set and if ICANON is set, the NL character is echoed, even if
 * ECHO is not set.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果来看,终端默认没有设置该标志位. */
    printf("term.c_lflag: %#x, ECHONL: %#x, &= %#x\n",
            term.c_lflag, ECHONL, term.c_lflag & ECHONL);

    term.c_lflag |= ECHONL;
    term.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_lflag: %#x, ECHONL: %#x, &= %#x\n",
            term.c_lflag, ECHONL, term.c_lflag & ECHONL);

    /* 开启ECHONL标志位后,执行cat命令,没有看出来NL是怎么被回显的.
     * 根据书中的描述,想到关闭ECHO标志位,然后发现,此时输入是没有
     * 回显的,按下回车后,会先换行,再回显出刚才输入的内容.那么这个
     * "换行"就是NL回显的体现.对比"1-echo"目录里面的程序,那里面
     * 关闭ECHO后,执行cat,按下回车,是直接在当前行回显输入的内容.
     * 而这里会先换行,也证明了NL被回显了.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
