#include <stdio.h>
#include <unistd.h>
#include <termios.h>

void whether_nl2cr(struct termios term)
{
    /* 查看终端输入是否映射 NL 为 CR */
    printf("**term.c_iflag: %#x, INLCR: %#x\n", term.c_iflag, INLCR);
    printf("**input: map NL to CR? ---- %s\n",
            (term.c_iflag & INLCR) ? "YES" : "NO");

    /* 查看终端输出是否映射 NL 为 CR */
    printf("##term.c_oflag: %#x, ONLCR: %#x\n", term.c_oflag, ONLCR);
    printf("##output: map NL to CR? ---- %s\n",
            (term.c_oflag & ONLCR) ? "YES" : "NO");
}

/* 终端I/O特殊字符 ---- NL
 * The newline character, which is also called the line delimiter. We
 * cannot change this character. This character is recognized on input
 * in canonical mode (ICANON). This character is returned to the reading
 * process.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从输出结果看,bash终端的输入默认没有映射 NL 为 CR.
     * 但是输出默认映射 NL 为 CR.
     */
    whether_nl2cr(term);

    /* 设置终端输入属性,将 NL 映射为 CR.
     * 目前尚不清楚将 NL 映射为 CR 会有什么影响.
     */
    term.c_iflag |= INLCR;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("after clear the INLCR flag\n");
    whether_nl2cr(term);

    /* 恢复终端原来的设置 */
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
