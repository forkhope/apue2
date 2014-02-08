#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>   /* umask() */
#include <sys/fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>   /* getrlimit() */
#include <sys/socket.h>
#include <netdb.h>      /* getaddrinfo(), gai_strerror() */
#include <syslog.h>     /* syslog() */

void daemonize(const char *cmd)
{
    struct rlimit rlim;
    int fd0, fd1, fd2;
    int i;
    pid_t pid;
    struct sigaction sa;

    if (getrlimit(RLIMIT_NOFILE, &rlim) < 0) {
        printf("%s: getrlimit error: %s", cmd, strerror(errno));
        exit(1);
    }

    umask(0);

    if ((pid = fork()) < 0) {
        printf("%s: fork error: %s", cmd, strerror(errno));
        exit(1);
    }
    else if (pid != 0) {    /* parent */
        exit(0);
    }
    setsid();   /* child */

    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        printf("%s: sigaction SIGHUP error: %s", cmd, strerror(errno));
        exit(1);
    }

    if ((pid = fork()) < 0) {
        printf("%s: fork error: %s", cmd, strerror(errno));
        exit(1);
    }
    else if (pid != 0) {    /* parent again */
        exit(0);
    }

    if (chdir("/") < 0) {
        printf("%s: chdir to / error: %s", cmd, strerror(errno));
        exit(1);
    }

    /* child */
    if (rlim.rlim_max == RLIM_INFINITY)
        rlim.rlim_max = 1024;
    for (i = 0; i < rlim.rlim_max; ++i)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors: %d, %d, %d",
                fd0, fd1, fd2);
        exit(1);
    }
}

int initserver(int type, const struct sockaddr *addr,
        socklen_t addrlen, int qlen)
{
    int sockfd, err = 0;

    if ((sockfd = socket(addr->sa_family, type, 0)) < 0) {
        perror("socket error");
        return -1;
    }

    if (bind(sockfd, addr, addrlen) < 0) {
        err = errno;
        goto errend;
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(sockfd, qlen) < 0) {
            err = errno;
            goto errend;
        }
    }
    return sockfd;

errend:
    close(sockfd);
    errno = err;
    return -1;
}

#define BUFLEN 128
void serve(int sockfd)
{
    int connfd;
    char buf[BUFLEN];
    FILE *fp;

    for (;;) {
        if ((connfd = accept(sockfd, NULL, NULL)) < 0) {
            syslog(LOG_ERR, "accept error: %s\n", strerror(errno));
            exit(1);
        }

        if ((fp = popen("/usr/bin/uptime", "r")) < 0) {
            sprintf(buf, "popen uptime error: %s\n", strerror(errno));
            send(connfd, buf, BUFLEN, 0);
        }
        else {
            while (fgets(buf, BUFLEN, fp) != NULL) {
                send(connfd, buf, strlen(buf), 0);
            }
            pclose(fp);
        }
        close(connfd);
        exit(0);
    }
}

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

#define QLEN 10

/* Since a socket endpoint is represented as a file descriptor, we can use
 * read() and write() to communicate with a socket, as long as it is
 * connected. Although we can exchange data using read() and write(), that
 * is about all we can do with these two functions. If we want to specify
 * options, receive packets from multiple clients, or send out-of-band data,
 * we need to use on of the six socket functions designed for data transfer.
 *
 * The simplest one is send(). It is similar to write(), but allows us to
 * specify flags to change how the data we want to transmit is treated.
 * #include <sys/socket.h>
 * ssize_t send(int sockfd, const void *buf, size_t nbytes, int flags);
 *      Returns: number of bytes sent if OK, -1 on error
 * Like write(), the socket has to be connected to use send(). The buf and
 * nbytes arguments have the same meaning as they do with write().
 * send()函数的flags可能取值为:MSG_DONTROUTE, MSG_DONTWAIT, MSG_EOR, MSG_OOB
 *
 * If send() returns success, it doesn't necessarily mean that the process
 * at the other end of the connection receives the data. All we are
 * guaranteed is that when send() succeeds, the data has been delivered to
 * the network drivers without error.
 *
 * With a protocol that supports message boundaries, if we try to send a
 * single message larger than the maximum supported by the protocol, send()
 * will fail with errno set to EMSGSIZE. With a byte-stream protocol, send()
 * will block until the entire amount of data has been transmitted.
 *
 * The sendto() function is similar to send(). The diffrence is that
 * sendto() allows us to specify a destination address to be used with
 * connectionless socket.
 * ssize_t sendto(int sockfd, const void *buf, size_t nbytes, int flags,
 *      const struct sockaddr *destaddr, socklen_t destlen);
 *      Returns: number of bytes sent if OK, -1 on error 
 * With a connection-oriented socket, the destination address is ignored, as
 * the destination is implied by the connection. With a connectionless
 * socket, we can't use send() unless the destination address is first set
 * by calling connect(), so sendto() gives us an alternate way to send a
 * message. 
 * man sento 手册对sendto()函数描述为: If sendto() is used to a connection-
 * mode (SOCK_STREAM, SOCK_SEQPACKET) socket, the arguments destaddr and
 * destlen are ignored (and the error EISCONN may be returned when they are
 * not NULL and 0), and the error ENOTCONN is returned when the socket was
 * not actually connected. Otherwise, the address of the target is given by
 * destaddr with addrlen specifying its size.
 * sendto()函数指定要发送到的地址.而recvfrom()函数并不指定要接收哪
 * 个地址发送过来的信息,其最后两个参数用来获取对方地址,也就是发送方的地址是
 * 不确定的.那么不同进程的不同recvfrom()函数之间用什么分辨哪个数据是发送到
 * 哪个进程的呢, 例如同时有两个进程都在调用recvfrom(),那么网络传输过来的数
 * 据会被哪个进程接收到? 说明如下:
 * 1.服务器一般会显式调用bind()函数来将sockfd和具体的地址、端口绑定起来.这
 * 样,使用sockfd作为参数调用recvfrom()函数时,recvfrom()函数就会接收到被发往
 * 该地址和端口的数据.而恰好客户端在发送数据时需要指定对方的地址和端口.
 * 2.客户端一般会先发送数据到服务器,在发送数据时,系统会为sockfd选择默认地址
 * 和端口,并把这个地址和端口连同数据一起发给服务器,服务器使用该地址和端口
 * 作为参数发送数据给客户端,由于直接指定了地址,所以客户端的recvfrom()函数
 * 能接收该数据,而不会和其他进程的recvfrom()函数发生冲突.
 * 3.经过试验,如果即没有调用bind(),也没有调用sendto(),直接就调用recvfrom(),
 * 不会报错,它会一直阻塞,目前不确定这种情况,该recvfrom()函数会接收到什么数
 * 据.算是一个遗留问题,有待查证.
 *
 * We have one more choice when transmitting data over a socket. We can call
 * sendmsg() with a msghdr structure to specify multiple buffers from which
 * to transmit data, similar to the writev() function.
 * ssize_t sendmsg(int sockfd, const struct msghdr *msg, int flags);
 *      Returns: number of bytes sent if OK, -1 on error
 * 关于这个函数的其他信息可查看man手册.
 */
int main(void)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, n, err;
    char *hostname;

/* To find out its address, the server needs to get the name of the host on
 * which it is running. Some systems don't define the _SC_HOST_NAME_MAX
 * constant, so we use HOST_NAME_MAX in this case. If the system doesn't
 * define HOST_NAME_MAX, we define it ourselves. POSIX.1 states that the
 * minimum value for the host name is 255 bytes, not including the
 * terminating null, so we define HOST_NAME_MAX to be 256 to include the
 * terminating null.
 * man sysconf 中对 HOST_NAME_MAX, _SC_HOST_NAME_MAX 解释如下:
 * Max length of a hostname, not including the terminating null byte, as
 * returned by gethostname(). Must not be less than _POSIX_HOST_NAME_MAX
 * (255).
 */
#ifdef _SC_HOST_NAME_MAX
    n = sysconf(_SC_HOST_NAME_MAX);
    if (n < 0)
#endif
        n = HOST_NAME_MAX;
    if ((hostname = malloc(n)) == NULL) {
        perror("malloc error");
        exit(1);
    }

    if (gethostname(hostname, n) < 0) {
        perror("gethostname error");
        exit(1);
    }

    daemonize("ruptimed");

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM; /* 只获取tcp类型的服务 */
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    /* 注意, "ruptime" 服务并不是标准服务,系统中默认是没有这个服务的,运行
     * 时,getaddrinfo()函数报错: Servname not supported for ai_socktype
     * 解决这个问题的方法是:将"ruptime"服务添加到/etc/services文件中,这样
     * getaddrinfo()函数就能获取到该服务,不会再报错.
     */
    if ((err = getaddrinfo(hostname, "ruptime", &hint, &ailist)) < 0) {
        syslog(LOG_ERR, "getaddrinfo error: %s\n", gai_strerror(err));
        exit(1);
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
                        aip->ai_addrlen, QLEN)) >= 0) {
            serve(sockfd);
            exit(0);
        }
    }

    exit(1);
}
