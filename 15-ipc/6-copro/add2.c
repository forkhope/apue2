#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

/* Simple filter to add two number. */
int main(void)
{
    int n, int1, int2;
    char line[1024];

#if 0
    /* 书中提到,如果使用标准I/O库来实现这个程序,则将会陷入死锁: The problem
     * is the default standard I/O buffering. When the program is invoked,
     * the first fgets() on the standard input causes the standard I/O
     * library to allocate a buffer and choose the type of buffering. Since
     * the standard input is a pipe, the standard I/O library defaults to
     * fully buffered. The same thing happens with the standard output.
     * While add2 is blocked reading from its standard input, the other
     * program is blocked reading from the pipe. We have a deadlock.
     * 当标准输入,标准输出关联终端设备时,它们一般是行缓冲,否则,它们是全缓冲.
     * 而全缓冲的实际I/O操作发生在标准I/O缓冲区满了的时候,所以使用标准I/O库
     * 来实现时,在我们输入的数据填满标准I/O缓冲区之前,下面的fgets()函数都不
     * 会返回.但是另外一个程序中,在写入一行到pipe中后,就会调用read()函数读
     * add2的标准输出,从而陷入死锁.
     *
     * 这个问题可以通过调用 setvbuf() 函数来设置标准输入、标准输出为行缓冲
     * 来解决.
     */
    setvbuf(stdin, NULL, _IOLBF, 0);
    setvbuf(stdout, NULL, _IOLBF, 0);
    while (fgets(line, 1024, stdin) != NULL) {
        if (sscanf(line, "%d%d", &int1, &int2) == 2) {
            printf("%d\n", int1 + int2);
        }
        else {
            printf("invalid args\n");
            exit(1);
        }
    }
#endif

    while ((n = read(STDIN_FILENO, line, 1024)) > 0) {
        line[n] = '\0';         /* null terminate */
        if (sscanf(line, "%d%d", &int1, &int2) == 2) {
            sprintf(line, "%d\n", int1 + int2);
            n = strlen(line);
            if (write(STDOUT_FILENO, line, n) != n) {
                printf("add2: write error: %m\n");
                exit(1);
            }
        }
        else {
            if (write(STDOUT_FILENO, "invalid args\n", 13) != 13) {
                printf("write STDOUT_FILENO error: %m");
                exit(1);
            }
        }
    }
    exit(0);
}
