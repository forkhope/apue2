#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: NOFLSH, c_lflag, POSIX.1
 * By default, when the terminal driver generates the SIGINT and SIGQUIT
 * signals, both the input and output queues are flushed. Also, when it
 * generates the SIGSUSP signal, the input queue is flushed. If the NOFLSH
 * flag is set, this normal flushing of the queues does not occur when
 * these signals are generated.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 从执行结果看,终端默认没有开启该标志位 */
    printf("term.c_lflag: %#x, NOFLSH: %#x, &= %#x\n",
            term.c_lflag, NOFLSH, term.c_lflag & NOFLSH);

    return 0;
}
