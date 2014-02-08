#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

void readwrite(void)
{
    char buf[8];
    int n;

    /* 这里不要写为while循环,避免MIN>0, TIME>0时会进入死循环 */
    if ((n = read(STDIN_FILENO, buf, sizeof(buf))) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write STDOUT_FILENO error");
            exit(1);
        }
    }
    else if (n == 0)
        printf("read encountered EOF\n");
    else {
        perror("read STDIN_FILENO error");
        exit(1);
    }
}

void set_min_time(int fd, struct termios *term, int min, int time)
{
    term->c_cc[VMIN] = min;
    term->c_cc[VTIME] = time;
    if (tcsetattr(fd, TCSAFLUSH, term) < 0) {
        perror("tcsetattr STDIN_FILENO error");
        exit(1);
    }
    printf("(%d, %d): term.c_cc[VMIN]: %d, term.c_cc[VTIME]: %d\n",
            min, time, term->c_cc[VMIN], term->c_cc[VTIME]);
    readwrite();
    printf("\n----- readwrite() end --------\n");
}

/* 终端特殊字符 ---- VMIN, VTIME
 * 下面的内容出自 man tcgetattr 里面.
 * VMIN: Minimum number of characters for noncanonical read (MIN).
 * VTIME: Timeout in deciseconds for noncanonical read (TIME).
 * 注意, 这两个值只在非经典模式下有效.
 * 
 * In noncanonical mode input is available immediately (without the user
 * having to type a line-delimiter character), and line editing is disabled.
 * The settings of MIN (c_cc[VMIN] and TIME (c_cc[VTIME]) determine the
 * circumstances in which a read() completes; there are four distinct cases:
 * (1)MIN == 0; TIME == 0: If data is available, read() returns immediately,
 *    with the lesser of the number of bytes available, or the number of
 *    bytes requested. If no data is available, read() returns 0.
 * (2)MIN > 0; TIME == 0: read() blocks until the lesser of MIN bytes or
 *    the number of bytes requested are available, and returns the lesser
 *    of these two values.
 * (3)MIN == 0; TIME > 0: TIME specifies the limit for a timer in tenths of
 *    a second. The timer is started when read() is called. read() returns
 *    either when at least one byte of data is available, or when the timer
 *    expires. If the timer expires without any input becoming available,
 *    read() returns 0.
 * (4)MIN > 0; TIME > 0: TIME specifies the limit for a timer in tenths of
 *    a second. Once an initial byte of input becomes available, the timer
 *    is restarted after each further byte is received. read() returns
 *    either when the lesser of the number of bytes requested or MIN byte
 *    have been read, or when the inter-byte timeout expires. Because the
 *    timer is only started after the initial byte becomes available, at
 *    least one byte will be read.
 */
int main(void)
{
    struct termios term, oldterm;

    if (tcgetattr(STDIN_FILENO, &term) < 0) {
        perror("tcgetattr STDIN_FILENO error");
        return 1;
    }
    oldterm = term;

    /* 刚开始写代码时,忘记将终端设为非经典模式,然后下面一直不超时.
     * VMIN, VTIME 这两个值是在非经典模式下才生效的.
     */
    term.c_lflag &= ~ICANON;
    printf("first: term.c_cc[VMIN]: %d, term.c_cc[VTIME]: %d\n",
            term.c_cc[VMIN], term.c_cc[VTIME]);

    /* MIN == 0, TIME == 0. 根据上面的描述,如果此时没有输入,read()会
     * 立刻返回.从执行结果,也是会立刻就返回,且返回值是0,遇到了EOF.
     */
    set_min_time(STDIN_FILENO, &term, 0, 0);

    /* MIN == 3, TIME == 0. 根据上面的描述, read()函数将会一直阻塞,直到
     * 读取了 3 个字符为止.从执行结果看,无法做到输入更多的字符,当输入第
     * 三个字符时,read()立刻就会返回.结束本次read().由于read()函数返回
     * 后,就无法再输入,我们甚至无法输入第四个字符,也就说,不存在剩余的字
     * 符,以供下一次的read()函数读取.
     */
    set_min_time(STDIN_FILENO, &term, 3, 0);

    /* MIN == 0, TIME == 20, 2秒超时. 此时,定时器会在read()函数被调用时,
     * 立刻启动.如果指定时间内都没有字符,read()会返回0. 如果在指定时间内
     * 有字符输入,read()函数会读取到输入的字符并返回.按照上面的描述,它会
     * 读取到至少一个字符,但是从执行结果来看,它每次都只能读取到一个字符,
     * read()函数会在读取到一个字符后立刻返回,无法读取到多个字符.
     */
    set_min_time(STDIN_FILENO, &term, 0, 20);

    /* MIN == 3, TIME == 20, 2秒超时. 此时,定时器会在读取到第一个字符后才
     * 启动,而不是在调用read()函数时就立刻启动,且每次读取到一个字符时,定
     * 时器会重置,从新开始计时.所以如果第一个字符一直不输入的话,read()函数
     * 将会一直阻塞.从执行结果看,read()函数也是在读取到第三个字符后,立刻
     * 返回,无法输入多余的字符以供下一次的read()函数读取
     */
    set_min_time(STDIN_FILENO, &term, 3, 20);

    /* MIN == 0, TIME == 0. 如上所述,由于上面无法输入多余的字符,所以下面
     * 下面还是会立刻读取到EOF,读取不到上次输入剩余的字符.因为不存在所谓
     * 的剩余字符. man手册中描述的"the number of bytes available",目前
     * 还不清楚这些"可用字符"是怎么来的.
     */
    set_min_time(STDIN_FILENO, &term, 0, 0);

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &oldterm) < 0) {
        perror("tcsetattr STDIN_FILENO, error");
        return 1;
    }
    return 0;
}
