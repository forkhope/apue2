#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志: BRKINT: c_iflag, POSIX.1
 * If this flag is set and IGNBRK is not set, the input and output queues
 * are flushed when a BREAK is received, and a SIGINT signal is generated.
 * This signal is generated for the foreground process group if the terminal
 * device is a controlling terminal.
 *
 * If neither IGNBRK nor BRKINT is set, then a BREAK is read as a signal
 * character \0, unless PARMRK is set, in which case the BREAK is read as
 * the 3-byte sequence \377, \0, \0.
 *
 * 试着执行cat命令,然后按键盘上的Break键,但没看到有什么反应.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_iflag: %#x, BRKINT: %#x, &= %#x\n",
            term.c_iflag, BRKINT, term.c_iflag & BRKINT);
    printf("term.iflag: %#x, IGNBRK: %#x, &= %#x\n",
            term.c_iflag, IGNBRK, term.c_iflag & IGNBRK); 

    return 0;
}
