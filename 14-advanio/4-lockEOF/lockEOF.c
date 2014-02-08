#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define write_lock(fd, start, whence, len) \
    lock_reg((fd), F_SETLK, F_WRLCK, (start), (whence), (len))
#define un_lock(fd, start, whence, len) \
    lock_reg((fd), F_SETLK, F_UNLCK, (start), (whence), (len))

int lock_reg(int fd, int cmd, short type, off_t start,
        short whence, off_t len)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_start = start;
    fl.l_whence = whence;
    fl.l_len = len;

    return fcntl(fd, cmd, &fl);
}

/* Use caution when locking or unlocking relative to the end of file. When
 * a portion of a file is locked, the kernel converts the offset specified
 * into an absolute file offset. In addition to specifying an absolute file
 * offset (SEEK_SET), fcntl() allows us to specify this offset relative to
 * a point in the file: current (SEEK_CUR) or end of file (SEEK_END). The
 * kernel needs to remember the lock independent of the current file offset
 * or end of file, because the current offset and end of file can change,
 * and changes to these attributes shouldn't affect the state of existing
 * locks. 当程序使用相对位置SEEK_CUR或者SEEK_END来定位时,要小心的时,这两个
 * 位置可能会发生变化,例如执行read()或者write()函数后,当前文件偏移就会改变,
 * SEEK_CUR所指向的位置也就发生了改变,此后,SEEK_CUR所对应的位置就和之前代码
 * 中用到的SEEK_CUR所对应的位置有所不一样了,要小心处理这种情况.
 */
int main(void)
{
    pid_t pid;
    int fd;

    /* creat()返回的文件描述符将是只写模式,而下面也都是对文件加写锁,
     * 不会报错,如果是加读锁就会有问题了.
     */
    if ((fd = creat("tempfile", FILE_MODE)) < 0) {
        printf("can't open tempfile: %s\n", strerror(errno));
        return 1;
    }

    /* 从文件EOF往后加写锁,后续写到文件末尾的数据都会被锁住.
     * It obtains a write lock from the current end of the file onward,
     * covering any future data we might append to the file. Assuming that
     * we are at end of file when we perform the first write(), that will
     * extend the file by one byte, and that byte will be locked.
     */
    if (write_lock(fd, 0, SEEK_END, 0) < 0) {
        printf("write_lock error: %s\n", strerror(errno));
        return 1;
    }

    /* 写入一个字符,由于上面新建文件时会截断文件内容,所以新写入的这个字符
     * 就是文件的第一个字符,并将文件大小扩展为1,此时,该字符被锁住.
     */
    if (write(fd, "a", 1) != 1) {
        printf("write 'a' error: %s\n", strerror(errno));
        return 1;
    }

    /* 对文件EOF往后解锁,后续写到文件末尾的数据不会被锁住.但是有个要注意的
     * 地方:在写入一个字符后,文件EOF的位置和上面write_lock()函数执行时的EOF
     * 位置已经不一样了,现在的EOF位置是在刚才写入的字符之后,所以解锁之后,
     * 刚才被锁住的字符还是处于加锁状态.下面的子进程试图对这个字符加锁时,会
     * 报错,errno为EAGAIN,表示子进程当前无法对该字符加锁.
     * 这里书中写的是 un_lock(fd, 0, SEEK_END); 应该是漏写了第四个参数,因为
     * 书中Figure 14.5上面给出的un_lock()定义中,该函数确实是有4个参数,第四
     * 个参数是len,而这个参数也是必须的,因为record locks是基于字符区间的锁
     * (byte-range locking),所以解锁的时候不仅要指定起始位置,寻址位置,也要
     * 指定结束位置,否则书中 "Locks at End of File" 这段描述解释不通.
     *
     * The unlock that follows has the effect of removing the locks for
     * future writes that append data to the file, but it leaves a lock on
     * the last byte in the file. When the second write occurs, the end of
     * file is extended by one byte, but this byte is not locked.
     *
     * If we intended to remove the lock covering the byte we wrote in the
     * first write, we could have specified the length as -1. Negative-
     * length values represent the bytes before the specified offset.
     *
     * 注意,lseek(fd, SEEK_END, 0)是将文件偏移指向文件末尾,此时文件偏移位于
     * 文件最后一个字符的后面,而不是指向最后一个字符,此时,如果调用write(),
     * 则写入的字符是追加到文件末尾,不会覆盖文件的最后一个字符.如后面所示.
     */
    if (un_lock(fd, 0, SEEK_END, 0) < 0) {
        printf("un_lock error: %s\n", strerror(errno));
        return 1;
    }

    /* 再写入一个字符,此时,写入的这个字符位于之前写入字符的后一个字节,
     * 且由于上面已经解锁,所以新写入的这个字符不会被锁住,下面的子进程
     * 能够会这个字符加锁.但是还是有个要注意的地方,由于第一个写入的字符
     * 还是被锁住,所以子进程在对第二个字符加锁时,字符区间不能也包含第
     * 一个字符,否则还是会报错,例如写为write_lock(0, SEEK_SET, 2);就还是
     * 会报错,因为这样写并不是只对第二个字符加锁的意思,而是对文件开头的
     * 两个字符加锁,是一个区间,但第一个字符还是被父进程锁住,所以报错.
     */
    if (write(fd, "b", 1) != 1) {
        printf("write 'b' error: %s\n", strerror(errno));
        return 1;
    }

    if ((pid = fork()) < 0) {
        printf("fork error: %m\n");
        return 1;
    }
    else if (pid == 0) {        /* child */
        /* 子进程试图对文件的第一个字符加锁,会获取不到该锁,报错返回,errno
         * 为EGAIN,下面也打印了strerror(EGAIN)的值来作为对比.
         */
        if (write_lock(fd, 0, SEEK_SET, 1) < 0) {
            printf("child: write_lock: error: %s\n", strerror(errno));
        }
        printf("child: strerror(EAGAIN): %s\n", strerror(EAGAIN));
        
        /* 子进程对文件的第二个字符加锁,只加锁这一个字符,能够获取到锁 */
        if (write_lock(fd, 2, SEEK_SET, 1) == 0) {
            printf("child, can lock the (0, SEEK_END, 0)\n");
            close(fd);
        }
        return 0;
    }

    /* parent.
     * 等待子进程先执行,避免父进程先执行并退出后,之前所加的锁被释放,则
     * 子进程再次试图加锁时,就能获取到锁,这不是程序想要的结果.
     */
    sleep(1);

    lseek(fd, SEEK_END, 0);      /* 下面追加 'c' 字符到文件末尾 */
    if (write(fd, "c", 1) != 1) {
        printf("write 'c' error: %s\n", strerror(errno));
        return 1;
    }
    return 0;
}
