#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: NLDLY, c_oflag, XSI, Linux
 * Newline delay mask. The values for the mask are NL0 or NL1.
 * [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
 * 目前不清楚该标志位具体的作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_oflag: %#x, NLDLY: %#x, &= %#x\n",
            term.c_oflag, NLDLY, term.c_oflag & NLDLY);

    switch (term.c_oflag & NLDLY) {
        case NL0:
            printf("NL0\n");
            break;
        case NL1:
            printf("NL1\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }

    return 0;
}
