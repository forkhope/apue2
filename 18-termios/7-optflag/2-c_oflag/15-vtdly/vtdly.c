#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: VTDLY, XSI, Linux
 * Vertical tab delay mask. The values for the mask are VT0 or VT1.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_oflag: %#x, VTDLY: %#x, &= %#x\n",
            term.c_oflag, VTDLY, term.c_oflag & VTDLY);

    switch (term.c_oflag & VTDLY) {
        case VT0:
            printf("VT0\n");
            break;
        case VT1:
            printf("VT1\n");
            break;
        default:
            printf("unknown\n");
            break;
    }

    return 0;
}
