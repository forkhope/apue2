#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- WERASE, not in POSIX.
 * The word-erase character. This character is recognized on input in
 * extended, canonical mode (both IEXTEN and ICANON flags set) and causes
 * the previous word to be erased. First, it skips backward over any white
 * space (spaces or tabs), then backward over the previous token, leaving
 * the cursor positioned where the first character of the previous token
 * was located. Normally, the previous token ends when a white space
 * character is encountered. Wh can change this, however, by setting the
 * ALTWERASE flag. This flag causes the previous token to end when the
 * first nonalphanumeric character is encountered. The word-erase character
 * is discarded when processes (i.e., it is not passed to the process).
 * 使用 WERASE 字符擦除单词时,光标会往后停留在空白字符的位置,例如输入
 * tian xia    you
 * 则输入 WERASE 字符后,会变成: tian xia    ,光标刚才停留在前面逗号的位置.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 打印出来term.c_cc[VWERASE]的十进制值是23,八进制值是027,根据
     * man tcgetattr可知,该字符对应的键位是Ctrl-W.
     */
    printf("term.c_cc[VWERASE]: %#o, %d\n", term.c_cc[VWERASE],
            term.c_cc[VWERASE]);
    system("cat");

    /* 将 WERASE 字符设置为 'w' */
    term.c_cc[VWERASE] = 'w';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("after set the WERASE character to 'w'\n");
    system("cat");

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
