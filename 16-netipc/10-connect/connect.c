#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>     /* struct sockaddr_in */
#include <unistd.h>

/* This function shows what is known as an exponential backoff algorithm.
 * If the call to connect() fails, the process goes to sleep for a short
 * time and then tries again, increasing the delay each time through the
 * loop, up to a maximum delay of about 2 minutes.
 */
int connect_retry(int fd, const struct sockaddr *addr, socklen_t len)
{
    int nsecs;
    const int MAXSLEEP = 8; // 128

    /* Try to connect with exponential backoff. */
    for (nsecs = 1; nsecs <= MAXSLEEP; nsecs <<= 1) {
        if (connect(fd, addr, len) == 0)
            return 0;   /* connection accepted. */
        
        /* Delay before trying again. */
        if (nsecs <= MAXSLEEP / 2) {
            printf("=========: connect_retry: will sleep %d seconds\n", nsecs);
            sleep(nsecs);
        }
    }
    return -1;
}

/* If we're dealing with a connection-oriented network serivce (SOCK_STREAM
 * or SOCK_SEQPACKET), then before we can exchange data, we need to create
 * a connection between the socket of the process requesting the service
 * (the client) and the process providing the service (the server). We use
 * the connect() function to create a connection.
 * #include <sys/socket.h>
 * int connect(int sockfd, const struct sockaddr *addr, socklen_t len);
 *      Returns: 0 if OK, -1 on error
 * The address we specify with connect() is the address of the server with
 * which we wish to communicate. If sockfd is not bound to an address,
 * connect() will bind a default address for the caller.
 * When we try to connect to a server, the connect request might fail for
 * several reasons. Thus, applications must be able to handle connect()
 * error returns that might be caused by trnsient conditions. 即,connect()
 * 报错后,不会立刻就进行错误处理,而是再多尝试几次,可以采用上面提到的指数级
 * 递进的等待时间算法.
 *
 * If the socket descriptor is in nonblocking mode, connect() will return
 * -1 with errno set to the special error code EINPROGRESS if the connection
 * can't be established immediately. The application can use either poll()
 * or select() to determine when the file descriptor is writable. At this
 * point, the connection is complete.
 *
 * The connect() function can also be used with a connectionless network
 * service (SOCK_DGRAM). If we call connect() with a SOCK_DGRAM socket, the
 * destination address of all messages we send is set to the address we
 * specified in the connect() call, relieving us from having to provide
 * the address every time we transmit a message. In addition, we will
 * receive datagrams only from the address we've specified.
 */
int main(void)
{
    int sockfd;
    struct sockaddr_in sin, srv, cli;
    char buf[INET_ADDRSTRLEN];
    socklen_t socklen;

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket, AF_INET, SOCK_STREAM error");
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9527);
    if (inet_pton(AF_INET, "192.168.88", &sin.sin_addr.s_addr) < 0) {
        perror("inet_pton: AF_INET error");
        return 1;
    }

    if (connect_retry(sockfd, (struct sockaddr *)&sin, sizeof(sin)) < 0) {
        perror("connect error");
        return 1;
    }

    socklen = sizeof(cli);
    if (getsockname(sockfd, (struct sockaddr *)&cli, &socklen) < 0) {
        perror("getsockname error");
        return 1;
    }

    if (inet_ntop(AF_INET, &cli.sin_addr.s_addr, buf, sizeof(buf)) < 0) {
        perror("inet_ntop error");
        return 1;
    }
    printf("=========: client: address: %s\n", buf);

    printf("=========: client: network port: %#x, host port: %#x\n",
            cli.sin_port, ntohs(cli.sin_port));

    socklen = sizeof(srv);
    if (getpeername(sockfd, (struct sockaddr *)&srv, &socklen) < 0) {
        perror("getpeername error");
        return 1;
    }

    if (inet_ntop(AF_INET, &srv.sin_addr.s_addr, buf, sizeof(buf)) < 0) {
        perror("inet_ntop error");
        return 1;
    }
    printf("=========: server: address: %s\n", buf);

    printf("=========: server: network port: %#x, host port: %#x\n",
            srv.sin_port, ntohs(srv.sin_port));

    return 0;
}
