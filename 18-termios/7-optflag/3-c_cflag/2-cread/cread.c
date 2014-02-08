#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void readwrite(void)
{
    int n;
    char buf[BUFSIZ];

    while ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write STDOUT_FILENO error");
            exit(1);
        }
    }
    if (n == 0)
        printf("read: encounter EOF\n");
    else {
        perror("read STDIN_FILENO error");
        exit(1);
    }
}

/* 终端标志位: CREAD, c_cflag, POSIX.1
 * If set, the receiver is enabled, and characters can be received.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;
    
    printf("term.c_cflag: %#x, CREAD: %#x, &= %#x\n",
            term.c_cflag, CREAD, term.c_cflag & CREAD);

    /* 下面打算关闭 CREAD 标志位,代码可以编译通过,但是运行时报错:
     * tcsetattr STDIN_FILENO error: Invalid argument
     * 看来,CREAD标志位不能轻易关闭.
     */
    // term.c_cflag &= ~CREAD;
    // if (tcsetattr(STDIN_FILENO, TCSANOW, &term) < 0) {
    //     perror("tcsetattr STDIN_FILENO error");
    //     return 1;
    // }
    // readwrite();

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("final: tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
