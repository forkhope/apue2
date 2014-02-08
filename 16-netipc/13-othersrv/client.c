#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> /* sleep() */
#include <sys/socket.h>
#include <netdb.h>  /* getaddrinfo(), gai_strerror() */

#define MAXADDRLEN 256
#define BUFLEN 128

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t len)
{
    const int MAXTIMES = 8;
    int nsecs;

    for (nsecs = 1; nsecs <= MAXTIMES; nsecs <<= 1) {
        if (connect(sockfd, addr, len) == 0) {
            return 0;
        }

        if (nsecs <= MAXTIMES / 2)
            sleep(nsecs);
    }

    return -1;
}

/* The recv() function is similar to read(), but allows us to specify some
 * options to control how we receive the data.
 * #include <sys/socket.h>
 * ssize_t recv(int sockfd, void *buf, size_t nbytes, int flags);
 *  Returns: length of message in bytes, 0 if no messages are available and
 *           peer has done an orderly shutdown, or -1 on error
 * flags参数可能的取值为: MSG_OOB, MSG_PEEK, MSG_TRUNC, MSG_WAITALL
 * When we specify the MSG_PEEK flag, we can peek at the next data to be
 * read without actually consuming it. The next call to read() or one of
 * the recv() functions will return the same data we peeked at.
 *
 * With SOCK_STREAM sockets, we can receive less data than we requested.
 * The MSG_WAITALL flag inhibits this behavior, preventing recv() from
 * returning until all the data we requested has been received. With
 * SOCK_DGRAM and SOCK_SEQPACKET sockets, the MSG_WAITALL flag provides no
 * change in behavior, because these message-based socket types already
 * return an entire message in a single read.
 *
 * If the sender has called shutdown() to end transmission, or if the
 * network protocol supports orderly shutdown by default and the sender has
 * closed the socket, then recv() will return 0 when we have received all
 * the data.
 *
 * If we are interested in the identify of the sender, we can use recvfrom()
 * to obtain the source address from which the data was sent.
 * ssize_t recvffrom(int sockfd, void *buf, size_t len, int flags,
 *  struct sockaddr *addr, socklen_t *addrlen);
 *  Returns: length of message in bytes, 0 if no messages are available and
 *           peer has done an orderly shutdown, or -1 on error
 * If addr is non-null, it will contain the address of the socket endpoint
 * from which the data was sent. When calling recvfrom(), we need to set the
 * addrlen parameter to point to an integer containing the size in bytes of
 * the socket buffer to which addr points. On return, the integer is set to
 * the actual size of the address in bytes. Because it allows us to retrieve
 * the address of the sender, recvfrom() is usually used with connectionless
 * sockets. Otherwise, recvfrom() behaves identically to recv().
 *
 * To receive data into multiple buffers, similar to readv(), or if we want
 * to receive ancillary data, we can use recvmsg().
 * ssize_t recvmsg(int sockfd, struct msghdr *msg, int flags);
 *  Returns: length of message in bytes, 0 if no messages are available and
 *           peer has done an orderly shutdown, or -1 on error
 * 关于这个函数,就不多作手打了,查看man手册即可.
 */
void print_uptime(int sockfd)
{
    char buf[BUFLEN];
    int n;

    while ((n = recv(sockfd, buf, BUFLEN, 0)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    if (n < 0) {
        perror("recv error");
        exit(1);
    }
}

/* Client command to get uptime from server.
 * This program connects to a server, reads the string sent by the server,
 * and prints the string on the standard output. Since we're using a
 * SOCK_STREAM socket, we can't be guaranteed that we will read the entire
 * string in one call to recv(), so we need to repeat the call until it
 * returns 0.
 */
int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err;

    if (argc != 2) {
        printf("usage: %s hostname\n", argv[0]);
        return 1;
    }

    hint.ai_flags = 0;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    /* 注意,这个 "ruptime" 的服务并不是标准服务,系统中默认是没有该服务的,
     * 所以程序运行的时候, getaddrinfo()函数报错:
     * Servname not supported for ai_socktype
     * 表示没有找到对应SOCK_STREAM类型的该服务.查找/etc/services文件,也
     * 确实没有找到该服务名.这个问题的解决方法是:将ruptime服务和端口号写入
     * /etc/services 文件中(注意用户自定义的端口号不能小于1024),将
     * ruptime      70000/tcp
     * 添加到 /etc/services 文件末尾, getaddrinfo()函数就不会报错.
     * 从这个例子中也可以看到,getaddrinfo()函数获取的内容和/etc/services
     * 文件存在联系.
     *
     * The getaddrinfo() function might return more than one candidate
     * address for us to use if the server supports multiple network
     * interfaces or multiple network protocols. We try each one in turn,
     * giving up when we find one that allows us to connect to the service.
     */
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0)
            err = errno;
        if (connect_retry(sockfd, aip->ai_addr, aip->ai_addrlen) < 0)
            err = errno;
        else {
            print_uptime(sockfd);
            return 0;
        }
    }

    printf("can't connect to %s: %s\n", argv[1], strerror(err));
    return 1;
}
