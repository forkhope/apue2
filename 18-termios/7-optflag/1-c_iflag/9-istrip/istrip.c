#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: ISTRIP, c_iflag, POSIX.1
 * When set, valid input bytes are stripped to 7 bits. When this flag
 * is not set, all 8 bits are processed.
 * 目前不清楚该标志位具体的作用.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_iflag: %#x, ISTRIP: %#x, &= %#x\n",
            term.c_iflag, ISTRIP, term.c_iflag & ISTRIP);

    return 0;
}
