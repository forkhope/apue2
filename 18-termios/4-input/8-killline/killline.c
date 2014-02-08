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

/* The kill character. (The name "kill" is overused; recall the kill()
 * function used to send a signal to a process. This character should be
 * called the line-erase character; it has nothing to do with signals.)
 * It is recognized on input in canonical mode (ICANON). It erases the
 * entire line and is discared when processed (i.e., it is not passed to
 * the process).
 * man tcgetattr手册中,对该字符的描述是This erases the input since the
 * last EOF or beginning-of-line.下面的例子测试了EOF的情况.
 */
int main(void)
{
    struct termios term;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }

    /* 打印出来的c_cc[VKILL]的值是025,十进制值则是21,根据man tcgetattr
     * 手册,可知025对应的ascii字符是NAK,其键位是Ctrl-U.也可通过下面的网址
     * pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap10.html
     * 来确认这一点.也就是说通过按下Ctrl-U来输入NAK字符.
     *
     * 假设我们输入了tian这四个字符,然后输入EOF,此时会立刻回显tian,且没有
     * 换行,屏幕上显示 "tiantian",光标就停留在字符'n'的后面,接着我们再输入
     * 若干个字符,然后输入VKILL字符,则刚才输入的若干个字符会被擦除,但是
     * 前面的"tiantian"并不会被擦除.这也是上面描述的,VKILL在擦除字符时,
     * 遇到EOF或者行首则终止.
     */
    printf("term.c_cc[VKILL]: %d, %#o\n", term.c_cc[VKILL],
            term.c_cc[VKILL]);
    read_write();

    term.c_cc[VKILL] = 'u';
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        return 1;
    }

    printf("########after set VKILL to 'u'\n");
    printf("term.c_cc[VKILL]: %d, %#o\n", term.c_cc[VKILL],
            term.c_cc[VKILL]);
    read_write();

    return 0;
}
