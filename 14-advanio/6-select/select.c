#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/select.h>

int fd, maxfdp1;
char buf[16];

void select_l(int nfds, fd_set *readfds, fd_set *writefds,
        fd_set *exceptfds, struct timeval *timeout)
{
    int err;

    if ((err = select(nfds,readfds,writefds,exceptfds,timeout)) < 0) {
        printf("select error: %s\n", strerror(errno));
        exit(1);
    }
    else if (err == 0) {
        printf("select timeout\n");
        exit(1);
    }
    else {
        printf("select ready: %d\n", err);

        /* 如果 select() 超时,它会返回0,此时那三个文件描述符集合都被会清零,
         * all the descriptor sets will be zeroed out.
         * 如果 select() 返回大于0的值,则那三个文件描述符集合中只保留已经
         * ready 了的文件描述符,那些没有 ready 文件描述符并会被清除,也就是
         * 说那些文件描述符已经不包含在那三个文件描述符集合中了. The only
         * bits left on in the three descriptor sets are the bits
         * corresponding to the descriptors that are ready. 此时,我们需要
         * 调用 FD_ISSET() 来对所有传入的文件描述符挨个检查,以得到 ready 了
         * 的文件描述符.
         */
        if (writefds != NULL && FD_ISSET(fd, writefds)) {
            printf("FD_ISSET(fd, writefds) true\n");
            write(fd, "abcde\n", 6);
        }

        if (readfds != NULL && FD_ISSET(fd, readfds)) {
            printf("FD_ISSET(fd, readfds) true\n");
            /* 上面调用 write() 函数后,文件偏移指针已经向前移动了,所以下面
             * 再调用read()函数时,并不会如预期那样读到刚才写入的"abcde\n",
             * 所以需要先将文件偏移指针定位到文件开始的地方,再读才能读到.
             */
            lseek(fd, 0, SEEK_SET);
            read(fd, buf, 4);
            buf[4] = '\0';
        }

        if (writefds != NULL && FD_ISSET(STDOUT_FILENO, writefds)) {
            printf("FD_ISSET(STDOUT_FILENO, writefds) true\n");
            printf("buf: %s\n", buf);
        }

        if (readfds != NULL && FD_ISSET(STDIN_FILENO, readfds)) {
            printf("FD_ISSET(STDIN_FILENO, readfds) true\n");
        }
    }
}

/* The select() function lets us do I/O multiplexing under all POSIX-
 * compatible platforms. The arguments we pass to select() tell the kernel
 * (1) Which descriptors we're interested in.
 * (2) What conditions we're interested in for each descriptor. (Do we want
 * to read from a given descriptor? Do we want to write to a given
 * descriptor? Are we interested in an exception condition for a given
 * descriptor?)
 * (3) How long we want to wait. (We can wait forever, wait a fixed amount
 * of time, or not wait at all.)
 * On the return from select(), the kernel tells us
 * (1) The total count of the number of descriptors that are ready
 * (2) Which descriptors are ready for each of the three conditions (read,
 * write, or exception condition)
 * #include <sys/select.h>
 * int select(int maxfdp1, fd_set *readfds, fd_set *writefds,
 *          fd_set *exceptionfds, struct timeval *tvptr);
 *          Returns: count of ready descriptors, 0 on timeout, -1 on error
 *
 * It is important to realize that whether a descriptor is blocking or not
 * doesn't affect whether select() blocks. That is, if we have a nonblocking
 * descriptor that we want to read from and we call select() with a timeout
 * value of 5 seconds, select() will block for up to 5 seconds.
 * If we encounter the end of file on a descriptor, that descriptor is
 * considered readable by select(), We then call read() and it returns 0,
 * the way to signify end of file on UNIX systems. (Many people incorrectly
 * assume that select() indicates an exception condition on a descriptor
 * when the end of file is reached.)
 *
 * fd_set 支持的最大文件描述符个数是由 FD_SETSIZE 常量指定了.
 * 关于文件描述符什么时候是ready的,书中有描述,其中有一点是: File descriptors
 * for regular files always return ready for reading, writing, and exception
 * condition. 即,普通文件的读写操作总是被认为可读、可写的.
 * 书中对 select() 函数的描述非常好,段段都经典,以至于我不能把整个小节都手打
 * 一遍,有什么问题就去翻书吧.
 *
 * int pselect(int maxfdp1, fd_set *readfds, fd_set *writefds,
 *             fd_set exceptfds, const struct timespec *tsptr,
 *             const sigset_t *sigmask);
 * 相比select(), pselect()函数多了一个信号掩码集和一些其他的区别,看书,看书.
 */
int main(void)
{
    fd_set rset, wset;
    struct timeval tv;

    if ((fd = open("tempfile", O_RDWR | O_CREAT, 0644)) < 0) {
        printf("open tempfile error: %s\n", strerror(errno));
        return 1;
    }

    FD_ZERO(&rset);
    FD_SET(STDIN_FILENO, &rset);
    FD_SET(fd, &rset);  /* 添加一个普通文件的文件描述符到集合中 */

    FD_ZERO(&wset);
    FD_SET(STDOUT_FILENO, &wset);
    FD_SET(fd, &wset);

    tv.tv_sec = 5;
    tv.tv_usec = 100;

    maxfdp1 = fd + 1;
    /* 由于普通文件的文件描述符总是认为可读、可写的,所以下面的select()函数
     * 会立刻返回大于0的值,从输出结果中看到,下面的select()返回3,fd可读、可
     * 写, ready 两次, STDOUT_FILENO 可写, ready 一次, STDIN_FILENO 还没
     * 来得及接收用户输入, select() 函数就返回了.
     */
    select_l(maxfdp1, &rset, &wset, NULL, &tv);
    printf("after first select: tv.tv_sec: %d, tv.tv_usec: %d\n",
            tv.tv_sec, tv.tv_usec);

    /* 开始写的时候,下面没有加 FD_SET(STDIN_FILENO, &rset); 语句,然后发现
     * 运行结果总是不对, select() 每次都超时才返回,标准输入始终不可读? 后
     * 来想了一下,发现执行过一次 select() 后,传入的三个文件描述符集合会被
     * 改写,上面第一次调用的时候,由于 STDIN_FILENO 不可读,所以这个描述符
     * 已经从 rset 中被清除了,需要再次把它添加到 rset 里面,然后再select().
     */
    FD_CLR(fd, &rset);  /* 清除  rset 里面的 fd 文件描述符 */
    FD_SET(STDIN_FILENO, &rset);

    /* POSIX.1 allows an implementation to modify the timeval structure,
     * so after select() returns, you can't rely on the structure containing
     * the same values it did before calling select(). Linux 正是会改写这个
     * 参数的值,所以在调用过一次 select() 后,最好还是再设置一次 tv 的值.
     */
    tv.tv_sec = 5;
    tv.tv_usec = 0;
    select_l(STDIN_FILENO + 1, &rset, NULL, NULL, &tv);
    printf("after second select: tv.tv_sec: %d, tv.tv_usec: %d\n",
            tv.tv_sec, tv.tv_usec);

    return 0;
}
