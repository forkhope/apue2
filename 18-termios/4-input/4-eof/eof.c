#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
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

/* 终端I/O的特殊字符 ---- EOF
 * The end-of-file character. This character is recognized on input in
 * canonical mode (ICANON). When we type this character, all bytes waiting
 * to be read are immedistely passed to the reading process. If no bytes
 * are waiting to be read, a count of 0 is returned. Entering an EOF
 * character at the beginning of the line is the normal way to indicate an
 * end of file to a program. This character is discarded when processed in
 * canonical mode (i.e., it is not passed to the process).
 */
int main(void)
{
    struct termios term;

    if (isatty(STDIN_FILENO) == 0) {
        printf("STDIN_FILENO is not a tty\n");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_cc[VEOF]: %d, %#o\n", term.c_cc[VEOF], term.c_cc[VEOF]);
    read_write();

    /* 将EOF对应的字符设置为字符'a'. 注意的是,这个设置在终端内全局有效,而
     * 不仅仅是进程内有效,第一次运行该程序时,上面读到的EOF字符是Ctrl-D,下面
     * 设置EOF为Ctrl-B后,下次再运行该程序,上面读到的EOF字符将是'a'.即,
     * 这个程序执行结束后,它对终端做的设置并没有被恢复成原来的样子,而是会
     * 一直有效.在终端的命令行中,如果输入EOF,终端将会退出.此时输入'a'时,
     * 终端也确实是退出了.
     */
    term.c_cc[VEOF] = 'a';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("after set EOF to Control-B, read and write again\n");
    read_write();

    return 0;
}
