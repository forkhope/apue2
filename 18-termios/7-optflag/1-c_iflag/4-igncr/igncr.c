#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: IGNCR, c_iflag, POSIX.1
 * If set, a received CR characgter is ignored. If this flag is not set,
 * it is possible to translate the received CR into a NL character if the
 * ICRNL flag is set.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看, 终端默认没有开启 IGNCR 标志位 */
    printf("term.c_iflag: %#x, IGNCR: %#x, &= %#x\n",
            term.c_iflag, IGNCR, term.c_iflag & IGNCR);

    term.c_iflag |= IGNCR;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable IGNCR: term.c_iflag: %#x, IGNCR: %#x, &= %#x\n",
            term.c_iflag, IGNCR, term.c_iflag & IGNCR);

    /* 开启 IGNCR 标志位后,回车将完全失去作用,执行cat命令,输入回车
     * 没有反应.这个跟关闭 ICRNL 标志位不同.关闭ICRNL标志位,输入回车
     * 后不会立刻回显,而且会打印出"^M"两个字符,但是输入换行(Ctrl-J)
     * 后,回车之前的字符会被丢弃,没有被回显出来.而开启 IGNCR 标志位
     * 后,输入回车,不会回显,也不会答应"^M"两个字符,输入换行后,回车
     * 之前的字符不会被丢失,全都被回显出来.此时,甚至无法识别回车之
     * 前的字符,因为输入行里面没有什么标志来表示回车字符.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
