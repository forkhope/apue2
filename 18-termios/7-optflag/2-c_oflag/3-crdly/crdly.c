#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: CRDLY, c_oflag, XSI
 * Carriage return delay mask. The values for the mask are CR0, CR1, CR2,
 * or CR3. [requires _BSD_SOURCE or _SVID_SOURCE or _XOPEN_SOURCE]
 *
 * 目前不清楚这个标志位的具体作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    switch (term.c_oflag & CRDLY) {
        case CR0:
            printf("CR0\n");
            break;
        case CR1:
            printf("CR1\n");
            break;
        case CR2:
            printf("CR2\n");
            break;
        case CR3:
            printf("CR3\n");
            break;
        default:
            printf("unknown\n");
            break;
    }

    return 0;
}
