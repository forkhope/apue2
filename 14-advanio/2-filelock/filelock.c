#include <fcntl.h>
#include <sys/stat.h>   /* S_IRUSR, S_IWUSR, S_IRGRP, S_IROTH */
#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define FILE_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)

#define read_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_RDLCK, (offset), (whence), (len))
#define readw_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_RDLCK, (offset), (whence), (len))
#define write_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_WRLCK, (offset), (whence), (len))
#define writew_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLKW, F_WRLCK, (offset), (whence), (len))
#define un_lock(fd, offset, whence, len) \
    lock_reg((fd), F_SETLK, F_UNLCK, (offset), (whence), (len))

int lock_reg(int fd, int cmd, short type, off_t offset, 
        short whence, off_t len)
{
    struct flock fl;

    fl.l_type = type;           /* F_RDLCK, F_WRLCK, F_UNLCK */
    fl.l_start = offset;        /* byte offset, relative to l_whence */
    fl.l_whence = whence;       /* SEEK_SET, SEEK_CUR, SEEK_END */
    fl.l_len = len;             /* #bytes (0 means to EOF) */

    return fcntl(fd, cmd, &fl);
}

#define is_read_lockable(fd, offset, whence, len) \
    (lock_test((fd), F_RDLCK, (offset), (whence), (len)) == 0)
#define is_write_lockable(fd, offset, whence, len) \
    (lock_test((fd), F_WRLCK, (offset), (whence), (len)) == 0)

pid_t lock_test(int fd, short type, off_t offset, off_t whence, off_t len)
{
    struct flock fl;

    fl.l_type = type;           /* F_RDLCK, F_WRLCK, F_UNLCK */
    fl.l_start = offset;        /* byte offset, relative to l_whence */
    fl.l_whence = whence;       /* SEEK_SET, SEEK_CUR, SEEK_END */
    fl.l_len = len;             /* #bytes (0 means to EOF) */

    fcntl(fd, F_GETLK, &fl);

    if (fl.l_type == F_UNLCK)
        return 0;       /* false, region isn't locked by another proc */
    return fl.l_pid;    /* ture, return pid of lock owner */
}

void lockabyte(const char *name, int fd, off_t offset)
{
    if (writew_lock(fd, offset, SEEK_SET, 1) < 0) {
        printf("%s: writew_lock: error: %s\n", name, strerror(errno));
        exit(1);
    }
    printf("%s: got the lock, byte %ld\n", name, offset);
}

/* Example of deadlock detection.
 * Deadlock occus when two processes are each waiting for a resource that
 * the other has locked. The potential for deadlock exists if a process that
 * controls a locked region is put to sleep when it tries to lock another
 * region that is controlled by a different process.
 * When a deadlock is detected, the kernel has to choose one process to
 * receive the error return. In this example, the child was chosen, but
 * this is an implementation detail. On some systems, the child always
 * receives the error. On other systems, the parent always gets the error.
 * On some systems, you might even see the errors split between the child
 * and the parent as multiple lock attempts are made.
 * 书中强调, "record lock" 是针对多个进程而言,而不是针对同一个进程的多个锁,
 * 同一个进程对一个已经加锁了区域再次加锁时,能够获取到锁,且新锁将会替换老锁
 * 即使使用一个写锁替换读锁也能成功,而多个进程之间,已经被加了读锁的区域,其它
 * 进程是不能再对这个区域加写锁的,会报错(F_SETLK)或者被阻塞(F_SETLKW).
 */
int main(void)
{
    pid_t pid;
    int fd;

    /* Create a file and write two bytes to it.
     * 注意: To obtain a read lock, the descriptor must be open for reading;
     * to obtain a write lock, the descriptor must be open for writing.
     */
    if ((fd = creat("tempfile", FILE_MODE)) < 0) {
        printf("creat tempfile error: %s\n", strerror(errno));
        return 1;
    }

    if (write(fd, "ab", 2) != 2) {
        printf("write tempfile with ab error: %s\n", strerror(errno));
        return 1;
    }

    /* 下面演示死锁的造成,当死锁被检测到时,内核会终止某个进程来破坏这个
     * 死锁,然后fcntl()函数报错: Resource deadlock avoided.错误码:EDEADLK
     */
    if ((pid = fork()) < 0) {
        printf("fork error: %s\n", strerror(errno));
        return 1;
    }
    else if (pid == 0) {        /* child */
        lockabyte("child", fd, 0);
        sleep(2);
        lockabyte("child", fd, 1);
    }
    else {      /* parent */
        sleep(1);
        lockabyte("parent", fd, 1);
        lockabyte("parent", fd, 0);
    }

    return 0;
}
