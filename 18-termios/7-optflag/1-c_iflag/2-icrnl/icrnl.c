#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ICRNL, c_iflag, POSIX.1
 * If set and if IGNCR is not set, a received CR character is translated
 * into a NL charcter.
 * 注意: CR 字符对应的键位是Ctrl-M,以及回车键,而 NL 字符对应的是 Ctrl-J.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认开启了 ICRNL 标志位 */
    printf("term.c_iflag: %#x, ICRNL: %#x, &= %#x\n",
            term.c_iflag, ICRNL, term.c_iflag & ICRNL);

    term.c_iflag &= ~ICRNL;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("disable ICRNL: term.c_iflag: %#x, ICRNL: %#x, &= %#x\n",
            term.c_iflag, ICRNL, term.c_iflag & ICRNL);

    /* 关闭 ICRNL 标志位后,执行cat命令,此时输入回车键或者Ctrl-M,字符串
     * 将不会回显,而是需要输入Ctrl-J键,也就是NL字符.虽然输入回车后,字符串
     * 不会被回显,但是在此之前输入的字符串好像被会清掉,当再输入Ctrl-J时,
     * Ctrl-M之前输入的字符串不会随之回显,如下:
     * tian^Mxia^Myouqingren    /// 这是输入的字符串,里面的^M就是回车键
     * youqingren               /// 这是输入Ctrl-J后,回显的字符串
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
