#include <stdio.h>
#include <netinet/in.h> /* struct sockaddr_in 结构体 */
#include <sys/time.h>   /* struct timeval */
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>  /* inet_pton(), struct linger */
#include <sys/socket.h>
#include <errno.h>

/* The function in Figure 16.10 (指老版本的initserver()函数) fails to
 * operate properly when the server terminates and we try to restart it
 * immediately. Normally, the implementation of TCP will prevent us from
 * binding the same address until a timeout expires, which is usually on
 * the order of several minutes. Luckily, the SO_REUSEADDR socket option
 * allows us to bypass this restriction.
 * To enable the SO_REUSEADDR option, we set an integer to a nonzero value
 * and pass the address of the integer as the val argument to setsockopt().
 * We set the len argument to the size of an integer to indicate the size
 * of the object to which val points.
 */
int initserver(int type, struct sockaddr *addr, socklen_t len, int qlen)
{
    int sockfd, err;
    int reuse = 1;

    if ((sockfd = socket(addr->sa_family, type, 0)) < 0) {
        return -1;
    }

    /* SO_REUSEADDR: Reuse addresses in bind() if *val is nonzero.
     * 此时, *val 的值只用与判断是否为0,除此之外,别无他用.所以,只要
     * 将 *val 设置为非0即可,具体是什么值都可以.
     */
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse,
                sizeof(int)) < 0) {
        err = errno;
        goto end;
    }

    if (bind(sockfd, addr, len) < 0) {
        err = errno;
        goto end;
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(sockfd, qlen) < 0) {
            err = errno;
            goto end;
        }
    }
    return sockfd;

end:
    close(sockfd);
    errno = err;
    return -1;
}

/* The socket mechanism provides two socket-option interfaces for us to
 * control the behavior of sockets. One interface is used to set an option,
 * and another interface allows us to query the state of an option. We can
 * get and set three kinds of options:
 * 1. Generic options that work with all socket types
 * 2. Options that are managed at the socket level, but depend on the
 *    underlying protocols for support
 * 3. Protocol-specific options unique to each individual protocol
 * The Single UNIX Specification defines only the socket-layer options (the
 * first two option types in the preceding list).
 * 
 * We can set a socket option with the setsockopt() function.
 * #include <sys/socket.h>
 * int setsockopt(int sockfd, int level, int option,
 *      const void *val, socklen_t len);
 *      Returns: 0 if OK, -1 on error
 * The level argument identifies the protocol to which the option applies.
 * If the option is a generic socket-level option, then level is set to
 * SOL_SOCKET. Otherwise, level is set to the number of the protocol that
 * controls the option. Examples are IPPROTO_TCP for TCP options and
 * IPPROTO_IP for IP options.
 * 书中提供了通用socket级别的 options 选项列表.
 *
 * The val argument points to a data structure or an integer, depending on
 * the option. Some options are on/off switches. If the integer in nonzero,
 * then the option is enabled. If the integer is zero, then the option is
 * disabled. The len argument specifies the size of the object to which
 * val points.
 *
 * We can find out the current value of an option with the getsockopt()
 * function.
 * #include <sys/socket.h>
 * int getsockopt(int sockfd, int level, int option,
 *      void *val, socklen_t *lenp);
 *      Returns:0 if OK, -1 on error
 * Note that the lenp argument is a pointer to an integer. Before calling
 * getsockopt(), we set the integer to the size of the buffer where the
 * option is to be copied. If the actual size of the option is greater than
 * this size, the option is silently truncated. If the actual size of the
 * option is less that or equal to this size, then the integer is updated
 * with the actual size on return.
 */
int main(void)
{
    int sockfd;
    struct sockaddr_in addr;
    socklen_t lenp;
    int val;
    struct timeval tv;
    struct linger lg;

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9527);
    if (inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.s_addr) < 0) {
        perror("inet_pton, AF_INET, 127.0.0.1 error");
        return 1;
    }

    sockfd = initserver(SOCK_STREAM, (struct sockaddr *)&addr, 
            sizeof(addr), 10);
    if (sockfd < 0) {
        perror("initserver error");
        return 1;
    }
    else {
        printf("initserver: sockfd: %d\n", sockfd);
    }

    lenp = sizeof(int);
    /* SO_ACCEPTCONN: int, Return whether a socket is enabled for
     *  listening (getsockopt() only).
     * 从测试结果来看,如果可监听会返回1,不可监听应该会返回0.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_ACCEPTCONN,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_ACCEPTCONN:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_BROADCAST: int, Broadcast datagrams if *val is nonzero.
     * 这句话有两个意思,如果该选项用于setsockopt()函数,则 *val 非空表示
     * 设置该socket为广播数据报;如果该选项用于getsockopt()函数,则当*val
     * 被设置为非空时,表示该socket是广播数据报类型.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_BROADCAST,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_BROADCAST:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_DEBUG: int, Debugging in network drivers enabled if *val is
     * nonzero. 其含义同上.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_DEBUG,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_DEBUG:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_DONTROUTE: int, Bypass normal routing if *val is nonzero */
    if (getsockopt(sockfd, SOL_SOCKET, SO_DONTROUTE,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_DONTROUTE:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_ERROR: int, Return and clear pending socket error
     *  (getsockopt() only)
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_ERROR:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_KEEPALIVE: int, Periodic keep-alive messages enabled if *val
     * is nonzero.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_KEEPALIVE:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(struct linger);
    /* SO_LINGER: struct linger, Delay time when unsent messages
     * exist and socket is closed. 
     * 通过 man 7 socket 查看到 struct linger的定义为:
     * struct linger {
     *      int l_onoff;    // linger active
     *      int l_linger;   // how many seconds to linger for
     * };
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_LINGER,
                &lg, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_LINGER:	%d,		lenp: %d\n", lg.l_onoff, lenp);
    printf("SO_LINGER:	%d,		lenp: %d\n", lg.l_linger, lenp);

    lenp = sizeof(int);
    /* SO_OOBINLINE: int, Out-of-band data placed inline with normal
     * data if *val is nonzero.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_OOBINLINE,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_OOBINLINE:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_RCVBUF: int, The size in bytes of the receive buffer. */
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_RCVBUF:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_RCVLOWAT: int, The minimum amount of data in bytes to return
     * on a receive call.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVLOWAT,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_RCVLOWAT:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(struct timeval);
    /* SO_RCVTIMEO: struct timeval, The timeout value for a socket
     * receive call.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
                &tv, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    /* 打印出来的结果,这两个值都是0. man 7 socket 手册中描述为:
     * If the timeout is set to zero (the default) then the operation will
     * never timeout.
     */
    printf("SO_RCVTIMEO:	%ld,		lenp: %d\n",(long)tv.tv_sec,lenp);
    printf("SO_RCVTIMEO:	%ld,		lenp: %d\n",(long)tv.tv_usec,lenp);

    lenp = sizeof(int);
    /* SO_REUSEADDR: int, Reuse addresses in bind() if *val is nonzero */
    if (getsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_REUSEADDR:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_SNDBUF: int, The size in bytes of the send buffer */
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_SNDBUF:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(int);
    /* SO_SNDLOWAT: int, The minimum amount of data in bytes to transmit
     * in a send call.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDLOWAT,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_SNDLOWAT:	%d,		lenp: %d\n", val, lenp);

    lenp = sizeof(struct timeval);
    /* SO_SNDTIMEO: struct timeval, The timeout value for a socket
     * send call.
     */
    if (getsockopt(sockfd, SOL_SOCKET, SO_SNDTIMEO,
                &tv, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_SNDTIMEO:	%ld,		lenp: %d\n",(long)tv.tv_sec,lenp);
    printf("SO_SNDTIMEO:	%ld,		lenp: %d\n",(long)tv.tv_usec,lenp);

    lenp = sizeof(int);
    /* SO_TYPE: int, Identify the socket type (getsockopt() only)) */
    if (getsockopt(sockfd, SOL_SOCKET, SO_TYPE,
                &val, &lenp) < 0) {
        perror("getsockopt error");
        return 1;
    }
    printf("SO_TYPE:	%d,		lenp: %d,\t", val, lenp);
    if (val == SOCK_STREAM)
        printf("SOCK_STREAM\n");
    else if (val == SOCK_DGRAM)
        printf("SOCK_DGRAM\n");
    else
        printf("other type\n");


    return 0;
}
