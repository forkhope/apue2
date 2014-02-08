#include <stdio.h>
#include <string.h>
#include <stdlib.h>     /* posix_openpt */
#include <fcntl.h>      /* O_RDWR, O_WRONLY, O_RDONLY, etc. */
#include <unistd.h>     /* isatty() */

/* man posix_openpt 手册后面的NOTES提到: The posix_openpt() function is a
 * recent invention in POSIX. Some UNIX implementations that support System
 * V (aka UNIX 98) pseudo-terminals don't have this function, but it is
 * easy to implement. 即,UNIX98下,可以用下面的方法来实现posix_openpt()函数
 */
int posix_openpt_l(int flags)
{
    return open("/dev/ptmx", flags);
}

void write_and_read(int writefd, int readfd, char *str)
{
    int len = strlen(str);
    int n;
    char buf[BUFSIZ];

    printf("now, write to the writefd: %d\n", writefd);
    if (write(writefd, str, len) != len) {
        perror("write write_fd error");
        exit(1);
    }

    printf("now, read from the readfd: %d\n", readfd);
    if ((n = read(readfd, buf, sizeof(buf))) < 0) {
        perror("read read_fd error");
        exit(1);
    }
    buf[n] = '\0';
    printf("read buf: %s\n", buf);
}

/* posix_openpt - open a pseudo-terminal device
 * #include <stdlib.h> * #include <fcntl.h>
 * int posix_opept(int flags);
 * 
 * This function opens an unused pseudo-terminal master device, returning a
 * file descriptor that can be used to refer to that device.
 * The flags argument is a bit mask that ORs together zero or more of the
 * following flags:
 * O_RDWR:   Open the device for both reading and writing. It is usual to
 *           specify this flag.
 * O_NOCTTY: Do not make this device the controlling terminal for the
 *           process.
 *
 * On success, posix_openpt() returns a nonnegative file descriptor which is
 * the lowest numbered unused descriptor. On failure, -1 is returned.
 *
 * Calling posix_openpt() creates a pathname for the corresponding pseudo-
 * terminal slave device. The pathname of the slave device can be obtained
 * using ptsname(). The slave device pathname exists only as long as the
 * master device is open.
 * #include <stdlib.h>
 * char *ptsname(int fd);
 * The ptsname() function returns the name of the slave pseudo-terminal
 * device corresponding to the master referred to by fd.
 * On success, ptsname() returns a pointer to a string in static storage
 * which will be overwritten by subsequent calls. This pointer must not
 * be freed. On failure, a NULL pointer is returned.
 *
 * Before a slave pseudo-terminal device can be used, its permissions need
 * to be set so that it is accessible to applications. The grantpt()
 * function does just this. It sets the user ID of the slave's device node
 * to be the caller's real user ID and sets the node's group ID to an
 * unspecified value, usually some group that has access to terminal device.
 * The permissions are set to allow read and write acess to individual
 * owners and write access to group owners (0620).
 * #include <stdlib.h>
 * int grantpt(int fd);
 * int unlockpt(int fd);
 *      Both return: 0 on success, -1 on error
 * To change permission on the slave device node, grantpt() might need to
 * fork() and exec() a set-user-ID program. Thus, the behavior is
 * unspecified if the caller is catching SIGCHLD.
 *
 * The unlockpt() function is used to grant access to the slave pseudo-
 * terminal device, thereby allowing applications to open the device. By
 * preventing others from opening the slave device, applications setting up
 * the devices have an opportunity to initialize the slave and master
 * devices properly before they can be used.
 *
 * Note that in both grantpt() and unlockpt(), the file descriptor
 * argument is the file descriptor associated with the master pseudo-
 * terminal device.
 */
int main(void)
{
    int fd, slave_fd;
    char *slave_path;

    /* 根据 man posix_openpt 手册,该函数需要特性测试宏_XOPEN_SOURCE的值
     * 大于等于600. 在我的机子上,默认应该是没有满足该条件,编译时告警:
     * warning: implicit declaration of function 'posix_openpt'
     * 要消除这个告警的话,可以在Makefile添加选项: -D_XOPEN_SOURCE=601
     */
    if ((fd = posix_openpt(O_RDWR)) < 0) {
        perror("posix_openpt O_RDWR error");
        return 1;
    }
    printf("posix_openpt returns master fd: %d\n", fd);
    printf("isatty() ? %d\n", isatty(fd));
    close(fd);

    if ((fd = posix_openpt_l(O_RDWR)) < 0) {
        perror("posix_openpt_l O_RDWR error");
        return 1;
    }
    printf("posix_openpt_l returns master fd: %d\n", fd);
    printf("isatty() ? %d\n", isatty(fd));

    if ((slave_path = ptsname(fd)) == NULL) {
        perror("ptsname error");
        return 1;
    }
    /* 多次执行该程序,打印出来的slave device path都是 /dev/pts/6.
     * 到/dev/pts目录下查看,发现6这个文件一直都会存在. man手册说的:
     * The slave device pathname exists only as long as the master
     * device is open.应该是说,当主设备被关闭后,这个路径名就没有具体
     * 关联哪个主设备了,但是/dev/pts/6这个文件还是存在的,不会被删掉.
     */
    printf("slave_path: %s\n", slave_path);

    if (grantpt(fd) < 0) {
        perror("grantpt error");
        return 1;
    }

    if (unlockpt(fd) < 0) {
        perror("unlockpt error");
        return 1;
    }

    /* 当不执行 grantpt(), unlockpt() 就直接调用open()函数打开slave
     * device时,运行报错: Input/output error, 错误码是 EIO. 目前看来,
     * 执行grantpt(), unlockpt()后,用open()函数打开slave device即可.
     */
    if ((slave_fd = open(slave_path, O_RDWR)) < 0) {
        perror("open slave pseudo-terminal for O_RDWR error");
        return 1;
    }
    printf("open returns slave_fd: %d\n", slave_fd);

    /* 当程序打开主从伪终端后,往从属伪终端写入字符,然后从主伪终端读取
     * 字符时,将会读取刚才写到从属伪终端的字符.反之亦然.
     */
    write_and_read(slave_fd, fd, "tian");
    /* 下面这里,当写入的字符串是 "xia" 时,会发现程序一直阻塞在read()函数
     * 这里,也就是从属伪终端在从主伪终端上读取字符时,没有在读到字符后立刻
     * 返回.而将 "xia" 改成 "xia\n" 后,就不会阻塞在read()函数,从属伪终端
     * 在遇到\n后,返回到read()函数.这应该是因为从属伪终端上有一个terminal
     * line discipline导致的.
     */
    write_and_read(fd, slave_fd, "xia\n");

    close(fd);
    close(slave_fd);
    return 0;
}
