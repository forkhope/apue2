#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* We simply try one of the terminal-specific functions (that doesn't
 * change anything if it succeeds) and lock at the return value.
 */
static int isatty_l(int fd)
{
    struct termios ts;
    /* return 1 if no error (is a tty) */
    return (tcgetattr(fd, &ts) != -1);
}

static void whethertty(int fd)
{
    printf("isatty: fd %d: %s\n", fd, isatty(fd) ? "tty":"not a tty");
    printf("isatty_l: fd %d: %s\n", fd, isatty_l(fd) ? "tty":"not a tty");
}

/* isatty(): test whether a file descriptor refers to a terminal.
 * #include <unistd.h>
 * int isatty(int fd);
 * isatty() returns 1 if fd is an open file descriptor referring to a
 * terminal; otherwise 0 is returned.
 * 上面提供了 isatty() 函数的一个实现.
 */
int main(void)
{
    /* 执行该程序时,可以通过重定向来改变标准输入,标准输出,标准错误输出对
     * 应的文件,例如将标准输入重定向为普通文本文件就能看到不同的执行结果
     */
    whethertty(0);
    whethertty(1);
    whethertty(2);
    return 0;
}
