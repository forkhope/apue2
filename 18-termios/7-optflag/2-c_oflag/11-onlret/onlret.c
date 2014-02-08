#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ONLRET, c_oflag, XSI, Linux
 * If set, the NL character is assumed to perform the carriage return
 * function on output.
 * man tcgetattr中对 ONLRET 描述为:
 * ONLRET: Don't output CR.
 *
 * 从上面描述看,输入NL字符就能回车,类似于将NL映射为CR-NL? 但是后面的
 * 执行结果却不是这样,目前没有看到该标志位起到作用.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_oflag: %#x, ONLRET: %#x, &= %#x\n",
            term.c_oflag, ONLRET, term.c_oflag & ONLRET);

    // term.c_oflag &= ~ONLCR;
    term.c_oflag |= ONLRET;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable ONLRET: term.c_oflag: %#x, ONLRET: %#x, &= %#x\n",
            term.c_oflag, ONLRET, term.c_oflag & ONLRET);

    /* 开启 ONLRET 标志位后,没有看到该标志位的作用,因为没有开启该
     * 标志位时,输入NL也会回车换行.而关闭 ONLCR 标志后,NL没有映射
     * 为CR-NL,此时即使开了ONLRET,输入NL也只是换行,没有回车.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
