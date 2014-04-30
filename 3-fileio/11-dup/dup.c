#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

/* An existing file descriptor is duplicated by either of the following
 * functions.     #include <unistd.h>
 * int dup(int filedes);    int dup2(int filedes, int filedes2);
 * Both return: new file descriptor if OK, -1 on error.
 *
 * The new file descriptor returned by dup() is guaranteed to be the
 * lowest-numbered available file descriptor. With dup2(), we specify the
 * value of the new descriptor with the filedes2 argument. If filedes2 is
 * already open, it is first closed. If filedes equals filedes2, then
 * dup2() returns filedes2 without closing it.
 *
 * The new file descriptor that is returned as the value of the functions
 * shares the same file table entry as the filedes argument.
 *
 * 虽然, dup() 函数的返回值被确保是最小的可用文件描述符,但是具体值是不可预
 * 知的; 而 dup2() 函数执行后,返回的文件描述符就是第二个参数的值.
 *
 * 假设有 fd, assignfd, dupfd 三个 int 型变量, fd 是open()函数的返回值,再
 * 执行 assignfd = fd; dupfd = dup(fd); 则 assignfd 和 dupfd 的异同是:
 *     1. assignfd 和 fd 实际是同一个文件描述符,它们都指向进程文件描述符表
 *        的同一个表项,拥有相同的 file descriptor status,指向同一个文件表
 *        表项,拥有相同的 file status flags, assigned 的值和 fd 的值相等;
 *        而 dupfd 和 fd 是两个不同的文件描述符,指向进程文件描述符表的两个
 *        表项,拥有不同的 file descriptor status, 当然,它们指向同一个文件
 *        表表项,拥有相同的 file status flags, 读写 dupfd 和 fd 都会影响到
 *        同一个文件, dupfd 的值和 fd 的值不相等.
 *     2. 执行 close(fd) 或者 close(assigned),则 fd 和 assignfd 都会不可用
 *        但是,不管是执行 close(fd),还是执行 close(assignfd), dupfd 都可用
 *        同样, 执行 close(dupfd), fd 和 assignfd 都还可用.
 */
int main(void)
{
    int fd, assignvalue, dup2fd, fd2;
    int n;
    off_t offset;
    char buf[64];

    /* 复制 标准输出STDOUT_FILENO 到 fd,则 fd 指向标准输出文件 */
    if ((fd = dup(STDOUT_FILENO)) < 0) {
        printf("dup: dup error: %s\n", strerror(errno));
        exit(1);
    }
    /* 查看输出结果,可以看出, STDOUT_FILENO 和 fd 的值不相等 */
    printf("STDOUT_FILENO = %d, fd = %d\n", STDOUT_FILENO, fd);

    dup2fd = 5;     /* 为 dup2fd 赋值为 5 */
    /* 下面的 dup2() 语句指定把 fd文件描述符 的值复制到 文件描述符5.
     * 即此时,文件描述符 5 和 fd文件描述符指向相同的 file table entry
     */
    if (dup2(fd, dup2fd) < 0) {
        printf("dup: dup2 error: %s\n", strerror(errno));
        exit(1);
    }
    printf("fd = %d, dup2fd = %d\n", fd, dup2fd);

    /* 此时,对 fd 调用write() 函数,将向标准输出文件写入数据 */
    if (write(fd, "tianxia\n", 8) != 8) {
        printf("dup: can't write to %d: %s\n", fd, strerror(errno));
        exit(1);
    }
    /* 向文件描述符 5 写入数据,也会写入标准输出文件 */
    if (write(5, "tianxia\n", 8) != 8) {
        printf("dup: can't write to %d: %s\n", fd, strerror(errno));
        exit(1);
    }

    if ((fd = open("test", O_RDONLY)) < 0) {
        perror("open test error");
        exit(1);
    }
    /* 复制 文件描述符fd 到 文件描述符fd2. */
    if ((fd2 = dup(fd)) < 0) {
        perror("dup error");
        exit(1);
    }
    printf("fd = %d, fd2 = %d\n", fd, fd2);

    /* 用 文件描述符fd 作为read()函数的参数,读取12个字符 */
    if ((n = read(fd, buf, 12)) < 0) {
        perror("read fd error");
        exit(1);
    }
    printf("read return: %d\n", n);

    /* 执行完上面的read()函数后,文件的偏移指针将会往前移动12位.
     * 下面执行lseek(fd, 0, SEEK_CUR)语句来获取当前的文件偏移.
     */
    if ((offset = lseek(fd, 0, SEEK_CUR)) < 0) {
        perror("lseek error");
        exit(1);
    }
    printf("lseek fd=%d, the current offset is: %lu\n", fd, offset);

    /* 同样查看 文件描述符fd2 的文件偏移.虽然上面并没有使用fd2变量来
     * 调用read()或者write()函数,它的文件偏移应该保持不变.但是按照dup()
     * 函数的概念, fd2 将会和 fd 共享文件表,也就是共享文件偏移.所以下面
     * 获取到的文件偏移应该等于上面的 fd 的文件偏移,也就是都是12.
     * 查看执行结果,也确实如此.
     */
    if ((offset = lseek(fd2, 0, SEEK_CUR)) < 0) {
        perror("lseek error");
        exit(1);
    }
    printf("lseek fd2=%d, the current offset is: %lu\n", fd2, offset);
    close(fd);
    close(fd2);

    if ((fd2 = open("test", O_RDWR | O_CREAT, 0644)) < 0) {
        printf("dup: can't open the file (test): %m\n");
        exit(1);
    }
    /* 此时, 文件描述符 5 处于打开状态,使用 5 作为 dup2() 的第二个参数,
     * 则 dup2() 函数会先关闭 文件描述符5, 再复制新的文件描述符到 5.
     */
    if (dup2(fd2, 5) < 0) {
        printf("dup: can't dup2 fd: %d: %m\n", fd2);
        exit(1);
    }
    /* 此时,再向 文件描述符5 写入数据,不是写入标准输出文件,而是写入test */
    if (write(5, "tianxia\n", 8) != 8) {
        printf("dup: can't write to %d: %s\n", fd, strerror(errno));
        exit(1);
    }

    assignvalue = fd;    /* 将 fd  的值赋给 assignvalue */
    /* 关闭 fd 之后, fd 和 assignvalue 都不可用,但不影响 STDOUT_FILENO.
     * 关于这一点的证明之一是:下面两个 if 语句可以调用 printf() 函数把出错
     * 信息写到标准输出上.而后面关闭掉STDOUT_FILENO之后,调用 printf() 打印
     * 不出任何信息,因为标准输出已经关闭了,printf()无法将数据写到里面.
     */
    close(fd);
    if (write(fd, "tian\n", 5) != 5) {    /* 将会报错, errno 为 EBADF */
        printf("dup: can't write to fd: %d: %s\n", fd, strerror(errno));
    }
    if (write(assignvalue, "tian\n", 5) != 5) {    /* 将会报错 */
        printf("dup: can't write to assignvalue: %d: %m\n", assignvalue);
    }
    if (write(STDOUT_FILENO, "tian\n", 5) != 5) {    /* 可以正常读写 */
        printf("dup: can't write to STDOUT_FILENO: %m\n");
    }

    /* 为了证明, close(fd) 确实不影响到 STDOUT_FILENO,下面将调用 close()
     * 函数关闭 STDOUT_FILENO,再尝试写入数据,看是否会报错.这是防止有"标准输
     * 出文件关不了"的误解,认为close(fd)也会关STDOUT_FILENO,但没关成功.
     */
    if (close(STDOUT_FILENO) < 0) {    /* 关闭标准输出,不会报错,可以关掉 */
        printf("dup: can't close STDOUT_FILENO 111\n");
        exit(1);
    }
    /* 由于已经关闭标准输出,如果用printf()函数来打印下面的出错信息,屏幕上将
     * 什么都看不到,因为printf()关联的是标准输出,但是标准输出已经不可用了;
     * 此时可以将出错信息写到标准错误输出上,就能在屏幕上看到这些信息了.
     */
    if (write(STDOUT_FILENO, "xia\n", 4) != 4) {
        fprintf(stderr, "dup: can't write to STDOUT_FILENO 22: %m\n");
    }

    /* 虽然 STDOUT_FILENO 已经被关闭,但是由于 dup2fd 也关联标准输出文件,
     * 向 dup2fd 写入数据,将会写入标准输出,即在屏幕上显示出来.
     */
    if (write(dup2fd, "dup2fd\n", 7) != 7) {
        fprintf(stderr, "dup: can't write to dup2fd: %m\n");
        exit(1);
    }

    /* 根据上面的描述, dup() 函数复制的是已经存在的文件描述符,即已经打开
     * 的文件描述符,如果 dup() 函数的参数是一个没有打开的文件描述符,则该
     * 函数调用失败,函数返回 -1,且 errno 被置为 EBADF.
     */
    if ((fd = dup(22)) < 0)
        fprintf(stderr, "dup: can't dup the file descriptor 12: %m\n");

    /* 关闭 dup2fd,使其成为 "Bad file descriptor */
    if (close(dup2fd) < 0)
        fprintf(stderr, "dup: can't close the fd %d\n", dup2fd);
    // 和 dup() 函数一样,如果要复制的文件描述符没有打开,dup2()函数会报错
    if (dup2(dup2fd, 33) < 0)
        fprintf(stderr, "dup: can't dup2 the fd %d: %m\n", dup2fd);

    exit(0);
}
