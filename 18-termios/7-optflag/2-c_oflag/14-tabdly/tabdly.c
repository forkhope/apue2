#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void readwrite(void)
{
    int c;

    while ((c = getchar()) != EOF) {
        if (c == '\t')
            printf("##encounter TAB:%c##\n", c);
        else if (c == ' ')
            printf("##encounter SPACE:%c##\n", c);
        else
            printf("%c", c);
    }
}

/* 终端标志位: TABDLY, c_oflag, XSI, Linux
 * Horizontal tab delay mask. The values for the mark are TAB0,
 * TAB1, TAB2, or TAB3.
 *
 * The value XTAGS is equal to TAB3. This value causes the system to
 * expand tabs into spaces. the system assumes a tab stop every eight
 * spaces, and we can't change this assumption.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看, 终端默认没有设置该标志位 */
    printf("term.c_oflag: %#x, TABDLY: %#x, &= %#x\n",
            term.c_oflag, TABDLY, term.c_oflag & TABDLY);

    switch (term.c_oflag & TABDLY) {
        case TAB0:
            printf("TAB0\n");
            break;
        case TAB1:
            printf("TAB1\n");
            break;
        case TAB2:
            printf("TAB2\n");
            break;
        case TAB3:
            printf("TAB3\n");
            break;
        default:
            printf("UNKNOWN\n");
            break;
    }

    printf("XTABS: %#x, TAB3: %#x\n", XTABS, TAB3);

    readwrite();

    term.c_oflag &= ~TABDLY;
    term.c_oflag |= XTABS;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable XTABS ##########\n");
    /* 开启 XTABS 标志位后,从终端读到的还是TAB字符,只是该TAB字符被
     * 回显出来时,会被扩展为空格.书中说按8个空格来扩展,并不是说一个TAB
     * 会被扩展成8个空格,而是输入TAB后,光标停留在的列数是8的倍数.例如:
     * tian    xia     you     qing    ren
     * tian##encounter TAB:    ##
     * xia##encounter TAB:     ##
     * you##encounter TAB:     ##
     * qing##encounter TAB:    ##
     * ren
     * xia     ming    youa    f
     * xia##encounter TAB:     ##
     * ming##encounter TAB:    ##
     * youa##encounter TAB:    ##
     * f
     * 可以看到,第一行先输入"tian"四个字符,然后再输入TAB,光标停留在'x'
     * 字符的位置,而第二个行先输入"xia"三个字符,再输入TAB,光标停留在'm'
     * 字符的位置,即光标始终停留在第8行(行数从0开始).
     */
    readwrite();

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
