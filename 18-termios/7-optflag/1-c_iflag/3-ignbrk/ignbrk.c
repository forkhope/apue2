#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IGNBRK, c_iflag, POSIX.1
 * When set, a BREAK condition on input is ignored. See BRKINT for a way
 * to have a BREAK condition either generate a SIGINT signal or be read
 * as data.
 *
 * 我的键盘输入BREAK键没有看到过什么特别的反应
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_iflag: %#x, IGNBRK: %#x, &= %#x\n",
            term.c_iflag, IGNBRK, term.c_iflag & IGNBRK);

    return 0;
}
