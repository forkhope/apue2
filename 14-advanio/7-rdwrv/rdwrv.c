#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <unistd.h>

/* The readv() and writev() functions let us read into and write from
 * multiple noncontiguous buffers in a single function call. These
 * operations are called scatter read and gather write.
 * #include <sys/uio.h>
 * ssize_t readv(int filedes, const struct iovec *iov, int iovcnt);
 * ssize_t write(int fd, const struct iovec *iov, int iovcnt);
 *              Both return: number of bytes read or written, -1 on error
 * The second argument to both functions is a pointer to an array of
 * iovec structures:
 * struct iovec {
 *      void *iov_base;  // starting address of buffer
 *      size_t iov_len;  // size of buffer
 * };
 * The number of elements in the iov array is specified by iovcnt. It is
 * limited to IOV_MAX.
 * The writev() function gathers the output data from the buffers in order:
 * iov[0], iov[1], through iov[iovcnt-1]; writev() returns the total number
 * of bytes output, which should normally equal the sum of all the buffer
 * lengths.
 * The readv() function scatters the data into the buffers in order, always
 * filling one buffer before proceeding to the next. readv() returns the
 * total number of bytes that were read. A count of 0 is returned if there
 * is no more data and the end of file is encountered.
 */
int main(void)
{
    int fd, i, total;
    struct iovec iovbuf[3], readvbuf[3];
    char *buf[] = {"tian xia ", "you qing ren ", "zhong cheng juan shu"};
    char rbuf[1024];
    char *p;

    if ((fd = open("tempfile", O_RDWR | O_CREAT, 0644)) < 0) {
        printf("open tempfile error: %s\n", strerror(errno));
        return 1;
    }

    total = 0;
    for (i = 0; i < 3; ++i) {
        iovbuf[i].iov_base = buf[i];
        iovbuf[i].iov_len = strlen(buf[i]);
        total += iovbuf[i].iov_len;
    }

    if (writev(fd, iovbuf, 3) != total) {
        printf("writev error: %s\n", strerror(errno));
        return 1;
    }
    system("cat tempfile");
    putchar('\n');

    p = rbuf;
    for (i = 0; i < 3; ++i) {
        readvbuf[i].iov_base = p;
        readvbuf[i].iov_len = 15;
        p += 15;
    }

    lseek(fd, 0, SEEK_SET);

    /* 开始是写成下面的形式: readv(fd, iovbuf, 3), 结果运行的时候总是报错:
     * Bad address, 也就是错误码EFAULT, man 2 read 中对这个错误码的解释是:
     * buf is outside your accessible address space. 原因在于 iovbuf[i] 中
     * 的 iov_base 都是指向 buf[i], 而 buf[i] 是指针,指向字符常量区,不可写
     * 区域,所以才会一直报这个错.
     */
    // if (readv(fd, iovbuf, 3) < 0) {
    if ((total = readv(fd, readvbuf, 3)) < 0) {
        printf("read error: %s\n", strerror(errno));
        return 1;
    }

    /* 由于 readvbuf[i].iov_len 在上面统一设置为15,3个数组元素,则总长度是
     * 45,但是读到的字符串长度是42,第三个元素没有填充完,为了避免写了过多的
     * 字符到标准输出,使用 readv() 函数的返回值来控制所写的字符个数,每次
     * 先判断 total 的值是否大于 readvbuf[i].iov_len, 如果大于,则打印所有
     * 字符,然后 total 再减去 readvbuf[i].iov_len,再作比较,如果total 小于
     * readvbuf[i].iov_len 时,则只打印 total 个字符.
     */
    for (i = 0; i < 3; ++i) {
        printf("total: %d\n", total);
        /* iov_len 是 size_t 类型的,在32位机器上,size_t 一般是 unsigned int
         * 而在64位机器上,size_t 一般是 unsigned long int,所以下面要用 %lu.
         */
        printf("readvbuf[%d].iov_len: %lu\n", i, readvbuf[i].iov_len);
        write(STDOUT_FILENO, (char *)readvbuf[i].iov_base,
                total > readvbuf[i].iov_len ? readvbuf[i].iov_len: total);
        total -= readvbuf[i].iov_len;
        putchar('\n');
    }

    return 0;
}
