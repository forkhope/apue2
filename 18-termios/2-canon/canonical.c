#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(void)
{
    char buf[BUFSIZ];
    int n, fd;

    /* 标准输入关联到终端窗口,类似于终端设置.当不做任何设置时,终端
     * 默认是CANONICAL MODE,此时终端的输入按行为处理,每次调用read()
     * 函数会在读取到一行后返回.每行之间的分隔符是\n.所以,下面的会
     * 按行读取输入,并输出.即用户按下回车后,立刻就会回显用户刚才的
     * 输入.这和下面读取文本文件时的表现有所区别.
     */
    while ((n = read(STDIN_FILENO, buf, BUFSIZ)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write error");
            return 1;
        }
    }
    if (n < 0) {
        perror("read error");
        return 1;
    }
    else
        printf("======== read stdin: EOF ========\n");

    if ((fd = open("Makefile", O_RDWR)) < 0) {
        perror("open error");
        return 1;
    }

    /* 这里虽然用了while循环,但是实际上这个循环只执行一次,read()函数
     * 一次就读取到整个 "Makefile" 文件中的内容,后面的write()函数将
     * 内容写到标准输出上.这里的读取不再是按行读取. 和上面有所区别.
     */
    while ((n = read(fd, buf, BUFSIZ)) > 0) {
        if (write(STDOUT_FILENO, buf, n) != n) {
            perror("write error");
            return 1;
        }
    }
    if (n < 0) {
        perror("read error");
        return 1;
    }
    else
        printf("----- read tempfile: EOF ----- \n");

    close(fd);
    return 0;

