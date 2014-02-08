#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define read_lock(fd, start, whence, len) \
    lock_reg((fd), F_SETLK, F_RDLCK, (start), (whence), (len))
#define writew_lock(fd, start, whence, len) \
    lock_reg((fd), F_SETLKW, F_WRLCK, (start), (whence), (len))

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

/* Implied inheritance and release of locks. 锁的隐含继承和释放
 * Three rules govern the automatic inheritance and release of record locks.
 * 1. Locks are associated with a process and a file. This has two implica-
 * tions. The first obvious: when a process terminates, all its locks are
 * released. The second is far from obvious: whenever a descriptor is closed
 * and locks on the file referenced by that descriptor for that process are
 * released. 第二个含义是说,多个文件描述符指向同一个文件,并加了不同的锁,则
 * 其中的任意一个文件描述符被关闭之后,当前进程对该文件加的所有锁都会被释放,
 * 包括那些其他文件描述符对文件加的锁,也就是关闭某个文件描述符时,也会释放其
 * 他文件描述符所加的锁.
 * 2. Locks are never inherited by the child across a fork(). This means
 * that if a process obtains a lock and then calls fork(), the child is
 * considered another process with regard to the lock that was obtained by
 * the parent. This makes sence because locks are meant to prevent multiple
 * processes from writing to the same file at the same time. If the child
 * inherited locks across a fork(), both the parent and the child could
 * write to the same file at the same time.
 * 3. Locks are inherited by a new program across an exec. Note, however,
 * that if the close-on-exec flag is set for a file descriptor, all locks
 * for the underlying file are released when the descriptor is closed as
 * part of an exec.
 */
int main(void)
{
    pid_t pid;
    int fd1, fd2;

    /* 这个程序开始的时候,使用下面的语句来获得文件描述符fd1,然后后面执行
     * read_lock()时报错: Bad file descriptor. 即errno为EBADF, man fcntl
     * 中对这个错误码的解释为: fd is not an open file descriptor, or the
     * command was F_SETLK or F_SETLKW and the file descriptor open mode
     * doesn't match with the type of lock requested. 即,调用read_lock()
     * 来对文件加读锁,但是文件描述符fd1不是用读模式打开的.查看man creat
     * 可以证实这一点: creat() is equivalent to open() with flags equal
     * to O_CREAT | O_WRONLY | O_TRUNC. 即,creat()返回的文件描述符只具有
     * 写权限,所以用F_SETLK命令试图通过该文件描述符加读锁时报EBADF的错.
     * 使用open()函数和O_RDWR标志位打开文件后,没有再报 EBADF 的错.
     */
    // if ((fd1 = creat("tempfile", FILE_MODE)) < 0) {
    if ((fd1 = open("tempfile", O_RDWR | O_CREAT, FILE_MODE)) < 0) {
        printf("creat tempfile error: %s\n", strerror(errno));
        return 1;
    }

    if (read_lock(fd1, 0, SEEK_SET, 0) < 0) {
        printf("lock reg: error: %s\n", strerror(errno));
        return 1;
    }

    if ((pid = fork()) < 0) {
        printf("fork error: %s\n", strerror(errno));
        return 1;
    }
    else if (pid == 0) {        /* child */
        long ppid = (long)getppid();

        printf("child %ld: want to get the lock\n", ppid);
        /* fork()之后,子进程不会继承父进程的record locks,所以调用writew_lock
         * 后,子进程将会被阻塞.而父进程又一次打开tempfile文件,并关闭这个新的
         * 文件描述符,按照上面描述的规则,父进程中tempfile文件的所有record
         * locks都将被释放,子进程才结束阻塞,获取到锁,然后打印后面的语句.
         */
        writew_lock(fd1, 0, SEEK_SET, 0);
        printf("child %ld: get the lock\n", ppid);
        return 0;
    }

    /* parent */
    sleep(1);
    if ((fd2 = open("tempfile", O_RDWR)) < 0) {
        printf("open tempfile error: %s\n", strerror(errno));
        return 1;
    }

    printf("open the tempfile again: fd2: %d\n", fd2);
    close(fd2);    /* 虽然关闭的是fd2,但是fd1所加的读锁也会被释放 */
    printf("after parent close the fd2, child can get the record lock\n");

    return 0;
}
