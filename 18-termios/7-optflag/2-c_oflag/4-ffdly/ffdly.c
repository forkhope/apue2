#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: FFDLY, c_oflag, XSI, Linux
 * Form feed delay mask. The values for the mask are FF0 or FF1.
 * 目前不清楚这个标志位具体的作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_oflag: %#x, FFDLY: %#x, &= %#x\n",
            term.c_oflag, FFDLY, term.c_oflag & FFDLY);
    switch (term.c_oflag & FFDLY) {
        case FF0:
            printf("FF0\n");
            break;
        case FF1:
            printf("FF1\n");
            break;
        default:
            printf("unknown\n");
            break;
    }

    return 0;
}
