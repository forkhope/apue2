#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void printcsize(struct termios term)
{
    switch (term.c_cflag & CSIZE) {
        case CS5:
            printf("5 bits/byte\n");
            break;
        case CS6:
            printf("6 bits/byte\n");
            break;
        case CS7:
            printf("7 bits/byte\n");
            break;
        case CS8:
            printf("8 bits/byte\n");
            break;
        default:
            printf("unknown bits/byte\n");
            break;
    }
}

/* The field a given option is controlled by is usually not apparent from
 * the option name alone.
 * All the flags listed specify one or more bits that we turn on or clear,
 * unless we call the flag a mask. A mask defines multiple bits grouped
 * together from which a set of values is defined. We have a defined name
 * for the mask and a name for each value. For example, to set the character
 * size, we first zero the bits using the character-size mask CSIZE, and
 * then set one of the values CS5, CS6, CS7, or CS8.
 *
 * The following program demonstrates the use of these masks to extract a
 * value and to set a value.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    printcsize(term);

    /* 从上面的描述来看,部分标志占用了多个位,所以有个掩码位来全部清除
     * 这些位,之后再打开具体的某个位.例如下面就清除CSIZE对应的所有位,
     * 然后再单独或上CS8,也就是说,CS5和CS8对应的可能不是同一个bit位.
     */
    term.c_cflag &= ~CSIZE;     /* zero out the bits */
    term.c_cflag |= CS8;        /* set 8 bits/byte */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    printcsize(term);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
