#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: OLCUC, c_oflag, Linux
 * If set, map lowercase characters to uppercase characters on output.
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
    printf("term.c_oflag: %#x, OLCUC: %#x, &= %#x\n",
            term.c_oflag, OLCUC, term.c_oflag & OLCUC);

    term.c_oflag |= OLCUC;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    /* 上面开启了OLCUC标志位后,这里打印出来的字符也变成了大写 */
    printf("enable OLCUC: term.c_oflag: %#x, OLCUC: %#x, &= %#x\n",
            term.c_oflag, OLCUC, term.c_oflag & OLCUC);

    /* 开启 OLCUC 标志位后,即使没按下Caps Lock(大写锁定)键,输入的
     * 字符也是显示为大写,应该是因为所显示的输入字符其实是回显,所以
     * 也被映射成大写了.同时,输入字符,显示为大写,按下回车后,cat命令
     * 回显出来的字符也是大写的.例如:
     * ENABLE OLCUC: TERM.C_OFLAG: 0X7, OLCUC: 0X2, &= 0X2
     * TIAN XIA YOU QING REN
     * TIAN XIA YOU QING REN
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
