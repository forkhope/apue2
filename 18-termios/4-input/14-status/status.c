#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- STATUS
 * The BSD status-request character. This character is recognized on input
 * in extended, canonical mode (both IEXTEN and ICANON flags set) and
 * generates the SIGINFO signal, which is sento to all processes in the
 * foreground process group. Additionally, if the NOKERNINFO flag is not
 * set, status information on the foreground process group is also displayed
 * on the terminal. This character is discarded when processed (i.e., it
 * is not passed to the process)  Not in POSIX, not supported under Linux.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

#ifdef VSTATUS
    printf("term.c_cc[VSTATUS]: %#x, %d\n", term.c_cc[VSTATUS],
            term.c_cc[VSTATUS]);
#else
    printf("under Linux, VSTATUS isn't defined\n");
#endif /* VSTATUS */

    return 0;
}
