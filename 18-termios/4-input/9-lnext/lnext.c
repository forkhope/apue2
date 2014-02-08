#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void read_write(void)
{
    char buf[BUFSIZ];
    int n;

    while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write STDOUT_FILENO error");
            exit(1);
        }
    if (n < 0) {
        perror("read STDIN_FILENO error");
        exit(1);
    }
}

/* LNEXT: The literal-next character. This character is recognized on input
 * in extended mode (IEXTEN) and causes any special meaning of the next
 * character to be ignored. This works for all special characters listed
 * in this section. We can use this character to type any character to a
 * program. The LNEXT character is discarded when processed, but the next
 * character entered is passed to the process. Not in POSIX.
 * 即输入该字符后,下一个输入字符将被视作文本字符,而失去它可能具有的特殊
 * 含义,例如先输入LNEXT字符后,再输入Ctrl-C,此时进程不会收到SIGINT,因为
 * Ctrl-C将被视作文本字符,终端不会在接收到该字符后,给进程发送信号.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 打印出来的c_cc[VLNEXT]的值是026,十进制值为22,根据man tcgetattr
     * 手册可知,026对应的字符是SYN,其键位是Ctrl-V.
     */
    printf("term.c_cc[VLNEXT]: %d, %#o\n", term.c_cc[VLNEXT],
            term.c_cc[VLNEXT]);
    read_write();

    term.c_cc[VLNEXT] = 'u';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("########after set VLNEXT to 'u'\n");
    printf("term.c_cc[VLNEXT]: %d, %#o\n", term.c_cc[VLNEXT],
            term.c_cc[VLNEXT]);
    read_write();

    return 0;
}
