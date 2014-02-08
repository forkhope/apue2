#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define read_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))

int lock_reg(int fd, int cmd, short type, off_t offset,
        short whence, off_t len)
{
    struct flock fl;

    fl.l_type = type;
    fl.l_start = offset;
    fl.l_whence = whence;
    fl.l_len = len;

    return fcntl(fd, cmd, &fl);
}

void set_fl(int fd, int flags)
{
    int val;

    if ((val = fcntl(fd, F_GETFL, 0)) < 0) {
        printf("set_fl: fcntl F_GETFL error: %s\n", strerror(errno));
        exit(1);
    }

    val |= flags;       /* turn on flags */
    if (fcntl(fd, F_SETFL, val) < 0) {
        printf("set_fl: fcntl F_SETFL error: %s\n", strerror(errno));
        exit(1);
    }
}

/* advisory lock 类似于线程互斥锁,如果多个进程都按照锁的规则来读写文件,则它
 * 是有效的,但是进程不获取锁就直接读写文件时, advisory lock 是不能阻止这种
 * 情况的.所以相对就还有个强制锁,并不是所有系统都支持强制锁,Linux是支持的.
 * Mandatory locking causes the kernel to check every open, read, and write
 * to verify that the calling process isn't violating a lock on the file
 * being accessed. Mandatory locking is sometimes called enforcement-mode
 * locking. On Linux, if you want mandatory locking, you need to enable it
 * on a per file system basis by using the -o mand option to the mount
 * command. Mandatory locking is enabled for a particular file by turning
 * on the set-group-ID bit and turning off the group-execute bit. Since the
 * set-group-ID bit makes sense when the group-execute bit is off. 即,没有
 * 一个特定的函数来指定要加强制锁还是劝告锁,而是通过打开文件的set-group-ID
 * 标志位和关闭group execute标志位来表示对该文件加锁时,加的是强制锁.
 *
 * What happens to a process that tries to read or write a file that has
 * mandatory locking enabled and the specified part of the file is currently
 * read-locked or write-locked by another process? The answer depends on the
 * type of operation (read or write), the type of lock held by the other
 * process (read lock or write lock), and whether the descriptor for the
 * read or write is nonblocking.
 * 在Linux中,如果某个文件被其他进程加了强制锁后,如果open()函数指定了O_TRUNC
 * 标志位时,open()函数会立刻返回EAGAIN错误码,不管是否也指定了O_NONBLOCK位.
 */
int main(int argc, char *argv[])
{
    int fd;
    pid_t pid;
    char buf[5];
    struct stat statbuf;

    if (argc != 2) {
        printf("usage: %s filename\n", argv[0]);
        exit(1);
    }

    if ((fd = open(argv[1], O_RDWR | O_CREAT | O_TRUNC, FILE_MODE)) < 0) {
        printf("open error: %s\n", strerror(errno));
        exit(1);
    }
    if (write(fd, "abcdef", 6) != 6) {
        printf("write error: %s\n", strerror(errno));
        exit(1);
    }

    /* turn on set-group-ID and turn off group-execute.
     * 这样的话,对该文件加锁,加的就是强制锁.
     */
    if (fstat(fd, &statbuf) < 0) {
        printf("fstat error: %s\n", strerror(errno));
        exit(1);
    }
    if (fchmod(fd, (statbuf.st_mode & ~S_IXGRP) | S_ISGID) < 0) {
        printf("fchmod error: %s\n", strerror(errno));
        exit(1);
    }

    /* write lock entire file */
    if (write_lock(fd, 0, SEEK_SET, 0) < 0) {
        printf("write_lock error: %s\n", strerror(errno));
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("fork error: %s\n", strerror(errno));
        exit(1);
    }
    else if (pid > 0) { /* parent */
        if (waitpid(pid, NULL, 0) < 0) {
            printf("waitpid error: %s\n", strerror(errno));
            exit(1);
        }
    }
    else {      /* child */
        set_fl(fd, O_NONBLOCK);

        /* first let's see what error we get if region is locked */
        if (read_lock(fd, 0, SEEK_SET, 0) != -1) {      /* no wait */
            printf("child: read_lock successed\n");
            exit(1);
        }
        printf("read_lock of already-locked region returns: %d\n", errno);

        /* now try to read the mandatory locked file */
        if (lseek(fd, 0, SEEK_SET) < 0) {
            printf("child: lseek error: %s\n", strerror(errno));
            exit(1);
        }
        if (read(fd, buf, 2) < 0) {
            printf("child: read failed (mandatory locking works: %m\n");
            exit(1);
        }
        else {
            printf("read OK (no mandator locking), buf = %2.2s\n", buf);
        }
    }

    exit(0);
}
