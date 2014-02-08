#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void read_write(void)
{
    char buf[BUFSIZ];
    int n;

    while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0)
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write STDOUT_FILENO error");
            exit(1);
        }
    if (n < 0) {
        perror("read STDIN_FILENO error");
        exit(1);
    }
}

/* 终端I/O特殊字符 ---- ERASE, ERASE2.
 * ERASE: The erase character (backspace). This character is recognized on
 * input in canonical mode (ICANON) and erases the previous character in
 * the line, not erasing beyond the beginning of the line. This character
 * is discarded when processed in canonical mode (i.e., it is no passed to
 * the process).
 * ERASE2: The alternate erase character (backspace). This character is
 * treated exactly like the erase character (ERASE).
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 非常奇怪,这里打印出来的 term.c_cc[VERASE] 的值是127,对应的ascii
     * 字符DEL,该字符应该是键盘上的Delete键?因为backspace字符的值是8,
     * 键盘上的backspace键对应的应该是backspace字符.但是在读取标准输入时,
     * 按下Delete键并不能删除前面一个字符,也不能删除后面一个字符,它会显示
     * 出 "^[[3~" 这样一个字符串.但此时,按下backspace键可以删除行内前一个
     * 字符,感觉打印出来的c_cc[VERASE]字符的值和实际有效的值不一样.
     *
     * 2013-07-13 更新
     * pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap10.html
     * 根据上面的网址,知道DEL字符对应的键位是<control>-?, 也就是同时按下
     * Ctrl, Shift, 和 / 三个键来输入DEL字符,而不是键盘的Delete键.
     * 另外,从上面的网址可知,backspace字符对应的键位是<control>-H.
     */
    printf("term.c_cc[VERASE]: %d, %#o\n", term.c_cc[VERASE],
            term.c_cc[VERASE]);
    read_write();

    /* 将ERASE设置为'-'后,读取标准输入,并写标准输出.当在行内输入'-'字符
     * 时,会擦除前面一个字符,光标后退一列,这个删除只在行内有效,它不能
     * 删除上一行的内容,当光标到达行首时,将不再移动.
     */
    term.c_cc[VERASE] = '-';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }
    printf("term.c_cc[VERASE]: %d, %#o\n", term.c_cc[VERASE],
            term.c_cc[VERASE]);
    read_write();

    return 0;
}
