#include <stdio.h>
#include <time.h>
#include <stddef.h>     /* offsetof() */
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/un.h>     /* struct sockaddr_un */

#define QLEN    10

/* Create a server endpoint of a connection.
 * Returns fd if all OK,<0 on error.
 */
int srv_listen(const char *name)
{
    int fd, err, errval;
    struct sockaddr_un un;
    socklen_t addrlen;

    /* create a UNIX domain stream socket */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -1;
    }
    unlink(name);   /* in case it already exists */

    /* fill in socket address structure */
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, name);    // 可能会溢出
    addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    /* bind the name to the descriptor */
    if (bind(fd, (struct sockaddr *)&un, addrlen) < 0) {
        errval = -2;
        goto errout;
    }

    if (listen(fd, QLEN) < 0) { /* tell kernel we're a server */
        errval = -3;
        goto errout;
    }

    return fd;
errout:
    err = errno;
    close(fd);
    errno = err;
    return errval;
}

#define STALE   30  /* client's name can't be older than this (sec) */

/* Wait for a client connection to arrive, and accept it. We also obtain
 * the client's user ID from the pathname that it must bind before
 * calling us. Returns new fd if all OK, <0 on error.
 */
int srv_accept(int listenfd, uid_t *uidptr)
{
    int connfd, err, errval;
    time_t staletime;
    socklen_t addrlen;
    struct stat statbuf;
    struct sockaddr_un un;

    addrlen = sizeof(un);
    if ((connfd = accept(listenfd,(struct sockaddr *)&un,&addrlen)) < 0) {
        return -1;  /* often errno=EINTR, if signal caught */
    }
    printf("the return addrlen of accept() is %d\n", addrlen);
    printf("sizeof(sun_faimly) is %lu\n", sizeof(un.sun_family));

    /* 由于 accept() 函数返回的addrlen长度是整个struct sockaddr_un变量的
     * 有效长度,包括了第一个成员 sun_family 的字节数,所以要减去它,已得到
     * sun_path 字符串本身的长度.
     *
     * obtain the client's uid from its calling address.
     */
    addrlen -= offsetof(struct sockaddr_un, sun_path);  // len of pathname
    printf("after -= offset(), addrlen is %d\n", addrlen);

    /* 实际测试发现,不加这个'\0'也没有问题,可以正确打印出sun_path的值,
     * 且没有乱码,可能是accept()函数会传入的un参数先进行了清零.当然,
     * 程序不该抱有这种假设,还是需要明确的添加这个'\0'作为字符串终止符.
     */
    un.sun_path[addrlen] = '\0';    /* null terminate */
    printf("the return sun_path: %s\n", un.sun_path);

    if (stat(un.sun_path, &statbuf) < 0) {
        errval = -2;
        goto errout;
    }

    if (S_ISSOCK(statbuf.st_mode) == 0) {
        errval = -3;
        goto errout;
    }

    if ((statbuf.st_mode & (S_IRWXG | S_IRWXO)) ||
            (statbuf.st_mode & S_IRWXU) != S_IRWXU) {
        errval = -4;    /* is not rwx------ */
        goto errout;
    }

    staletime = time(NULL) - STALE;
    if (statbuf.st_ctime < staletime ||
            statbuf.st_mtime < staletime ||
            statbuf.st_atime < staletime) {
        errval = -5;    /* i-node is too old */
        goto errout;
    }

    if (uidptr != NULL)
        *uidptr = statbuf.st_uid;   /* return uid of caller */

    if (unlink(un.sun_path) < 0) {  /* we're done with pathname now */
        perror("srv: unlink error");
    }
    return connfd;
errout:
    err = errno;
    close(connfd);
    errno = err;
    return errval;
}

#define SRVNAME     "foo.socket"

int main(void)
{
    int listenfd, connfd, n;
    char buf[BUFSIZ];
    uid_t uid;

    if ((listenfd = srv_listen(SRVNAME)) < 0) {
        perror("srv: srv_listen error");
        return 1;
    }

    if ((connfd = srv_accept(listenfd, &uid)) < 0) {
        perror("srv: srv_accept error");
        return 1;
    }
    printf("the uid of client is: %d\n", (int)uid);

    /* 这里不要写为 while,否则recv()函数将会一直阻塞 */
    if ((n = recv(connfd, buf, BUFSIZ, 0)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    if (n < 0) {
        perror("srv: recv error");
        return 1;
    }
    printf("srv: recv success\n");

    if (send(connfd, "xia\n", 4, 0) != 4) {
        perror("srv: send error");
        return 1;
    }
    printf("srv: send success\n");

    return 0;
}
