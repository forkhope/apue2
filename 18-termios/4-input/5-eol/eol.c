#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端I/O特殊字符 ---- EOL, EOL2.
 * EOL: The additional line delimiter character, like NL. This character
 * is recognized on input in canonical mode (ICANON) and is returned to
 * the reading process; however, this character is not normally used.
 * EOL2: Another line delimiter character, like NL. This character is
 * treated identically to the EOL character. Not in POSIX.
 * man tcgetattr手册中提到, EOL,EOL2 字符默认的值都是0,没有对应字符.
 * 注意,设置了EOL字符后,回车符依然是行分隔符,EOL并不能取消回车符的功能.
 */
int main(void)
{
    struct termios term;
    char buf[BUFSIZ];
    int n;

    if (isatty(STDIN_FILENO) == 0) {
        printf("STDIN_FILENO is not a tty\n");
        return 1;
    }

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    printf("term.c_cc[VEOL]: %u,%#x\n", term.c_cc[VEOL], term.c_cc[VEOL]);
    printf("term.c_cc[VEOL2]: %u,%#x\n",term.c_cc[VEOL2],term.c_cc[VEOL2]);

    /* 将EOL修改为字符 'a' */
    term.c_cc[VEOL] = 'a';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("term.c_cc[VEOL]: %u,%#x\n", term.c_cc[VEOL], term.c_cc[VEOL]);

    /* 然后,读取标准输入,并写标准输出.此时,可以发现,当输入a时,跟输入回车
     * 一样,会立刻回显之前输入的字符,但是光标没有换行.例如输入xia时,会
     * 立刻显示"tiatia",且光标停在a的后面.
     * 但是输入回车时,会回显之前输入的字符,且光标换行.在上面的描述中,EOL
     * 是附加的行分隔符,也就是说,回车符依然是行分隔符,只不过是多了一个EOL
     * 字符来作为行分隔符.
     */
    while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write STDOUT_FILENO error");
            return 1;
        }
    if (n < 0) {
        perror("read STDIN_FILENO error");
        return 1;
    }

    return 0;
}
