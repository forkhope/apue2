#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

/* flags are file status flags to turn on */
void set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        printf("set_fl: fcntl F_GETFL error: %s\n", strerror(errno));
        exit(1);
    }

    val |= flags;        /* turn on flags */
    if (fcntl(fd, F_SETFL, val) < 0) {
        printf("set_fl: fcntl F_SETFL error: %s\n", strerror(errno));
        exit(1);
    }
}

/* flags are the file status to turn off */
void cls_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        printf("cls_fl: fcntl: F_GETFL error: %s\n", strerror(errno));
        exit(1);
    }

    val &= ~flags;       /* turn flags off */
    if (fcntl(fd, F_SETFL, val) < 0) {
        printf("cls_fl: fcntl: F_SETFL error: %s\n", strerror(errno));
        exit(1);
    }
}

char buf[500000];

/* Nonblocking I/O lets us issue an I/O operation, such as an open(),
 * read(), or write(), and not have it block forever. If the operation
 * cannot be completed, the call returns immediately with an error noting
 * that the operation would have blocked.
 * There are two ways to specify nonblocking I/O for a given descriptor.
 * 1. If we call open() to get the descriptor, we can specify the
 *    O_NONBLOCK flag.
 * 2. For a descriptor that is already open, we call fcntl() to turn on
 *    the O_NONBLOCK file status flag.
 *
 * 根据书中的描述,阻塞是针对慢速系统调用(slow system calls)而言的,这些慢速
 * 系统调用包括对pipes, terminal devices, network devices的操作,部分ioctl
 * 操作等,而磁盘I/O (disk I/O)的相关系统调用并不属于慢速系统调用,其描述为:
 * We also said that system calls related to disk I/O are not considered
 * slow, even though the read or write of a disk file can block the caller
 * temporarily.
 *
 * 所以这个程序将数据输出到普通文件和输出到终端会产生不同的结果,如main()
 * 函数中的描述所示.
 */
int main(void)
{
    int ntowrite, nwrite;
    char *ptr;

    ntowrite = read(STDIN_FILENO, buf, sizeof(buf));
    fprintf(stderr, "read %d bytes\n", ntowrite);

    set_fl(STDOUT_FILENO, O_NONBLOCK);  /* set nonblocking */

    ptr = buf;
    while (ntowrite > 0) {      /* 使用 while 循环轮询读写 */
        errno = 0;
        nwrite = write(STDOUT_FILENO, ptr, ntowrite);
        
        /* 假设这个程序编译出来的可执行文件是nonblock,则使用类似于
         *      ./nonblock < inputfile 2>stderr.out
         * 的命令来将标准错误输出重定向到文件stderr.out上,并将数据写到
         * 终端的标准输出时,查看生成的stderr.out文件,可见其中包含大量
         * 的出错信息,其中, nwrite等于-1, errno等于 11,也就是EAGAIN,可见
         * 将标准输出设置为O_NONBLOCK后,程序在输出大量数据到终端标准输出
         * 的过程中,write()函数多次报错. 这是因为终端的系统调用属于慢速
         * 系统调用,所以可能会发生长久阻塞的情况.而程序又将标准输出设置为
         * O_NONBLOCK,所以write()函数报错返回,提示说上次操作可能会被阻塞.
         *
         * 而使用类似于: ./nonblock < inputfile >temp.file 的命令来将数据
         * 写到普通文件时,可以看到执行结果只输出一次下面的语句:
         *      nwrite: = 500000, errno = 0
         * write()函数没有报错,一次性直接执行完了,这是因为写数据到普通文
         * 件属于磁盘I/O操作,不属于慢速系统调用,所以write()函数没有遇到
         * 长久阻塞的情况,也就不曾报错了.
         */
        fprintf(stderr, "nwrite: = %d, errno = %d\n", nwrite, errno);
        
        if (nwrite > 0) {
            ptr += nwrite;
            ntowrite -= nwrite;
        }
    }
    
    cls_fl(STDOUT_FILENO, O_NONBLOCK);  /* clear nonblocking */
    return 0;
}
