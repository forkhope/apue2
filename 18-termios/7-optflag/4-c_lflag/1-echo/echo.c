#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ECHO, c_lflag, POSIX.1
 * If set, input characters are echoed back to the terminal device. Input
 * characters can be echoed in either canonical or noncanonical mode.
 * 当不设置该标志位时,终端上将看不到输入的字符.
 *
 * 可以这么理解:终端交互,分为读通道,写通道,从终端输入时,字符被送到对端的读
 * 通道,对端收到字符后,将这些字符重新写回本端的读通道,终端读取到这些字符,把
 * 它显示出来,从而在本端回显刚才输入的字符.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printf("term.c_lflag: %#x, ECHO: %#x, &= %#x\n",
            term.c_lflag, ECHO, term.c_lflag & ECHO);

    term.c_lflag &= ~ECHO;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_lflag: %#x, ECHO: %#x, &= %#x\n",
            term.c_lflag, ECHO, term.c_lflag & ECHO);

    /* 关闭 ECHO 标志位后,执行cat命令,此时输入的字符将不被回显出来,
     * 直到输入回车后,输入的字符被写入标准输出,才能看到字符.
     */
    system("cat");

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
