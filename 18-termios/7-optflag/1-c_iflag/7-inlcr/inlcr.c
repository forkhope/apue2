#include <stdio.h>
#include <unistd.h>
#include <termios.h>

/* 终端标志位: c_iflag, POSIX.1
 * If set, a received NL character is translated into a CR character.
 */
void readwrite(void)
{
    int c;

    while ((c = getchar()) != EOF) {
        printf("##read c: %d\n", c);
        if (c == '\n')
            printf("##read c == '\\n'\n");
        else if (c == '\r')
            printf("##read c == '\\r'\n");
        else
            printf("%c\n", c);
    }
}

int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 从执行结果看,终端默认没有设置该标志位 */
    printf("term.c_iflag: %#x, INLCR: %#x, &= %#x\n",
            term.c_iflag, INLCR, term.c_iflag & INLCR);

    /* 终端默认没有设置INLCR标志位,此时执行 readwrite() 函数,输入回车
     * (Ctrl-M) 和 换行(Ctrl-J) 时,都会打印出 "##read c == '\n'",这是
     * 因为终端默认设置了ICRNL,将回车转换成换行,读取到的字符就是换行符.
     */
    readwrite();

    term.c_iflag |= INLCR;
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("enable INLCR: term.c_iflag: %#x, INLCR: %#x, &= %#x\n",
            term.c_iflag, INLCR, term.c_iflag & INLCR);

    /* 上面开启 INLCR 标志位后,此时输入换行(Ctrl-J),会输出"^M"两个字符,
     * 且readwrite()函数里面调用的getchar()函数不会立刻返回,这是因为此时
     * 终端还在读取输入,然后再输入回车(Ctrl-M),会打印出如下内容:
     * enable INLCR: term.c_iflag: 0x6d42, INLCR: 0x40, &= 0x40
     * ^M               //// 这里是先输入Ctrl-J,然后再输入Ctrl-M
     * ##read c: 13
     * ##read c == '\r'
     * ##read c: 10
     * ##read c == '\n'
     * 可以看到,先是读到一个回车字符,该回车符是在输入换行符(Ctrl-J)时被
     * 转换得到的,然后又读到一个换行字符,而该换行符却是在输入回车符
     * (Ctrl-M)时被转换得到的.
     */
    readwrite();

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    return 0;
}
