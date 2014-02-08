#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>     /* struct sockaddr_in 结构体 */
#include <arpa/inet.h>
#include <sys/socket.h>

int initserver(int type, const struct sockaddr *addr, socklen_t len, int n)
{
    int sockfd, err = 0;

    if ((sockfd = socket(addr->sa_family, type, 0)) < 0) {
        return -1;
    }

    if (bind(sockfd, addr, len) < 0) {
        err = errno;
        goto errend;
    }

    /* 面向连接类型的socket 才需要监听 */
    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(sockfd, n) < 0) {
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

/* A server announces that it is willing to accept connect requests by
 * calling the listen() function.
 * #include <sys/socket.h>
 * int listen(int sockfd, int backlog);
 *      Returns: 0 if OK, -1 on error
 * The backlog argument provides a hint to the system of the number of
 * outstanding connect requests that is should enqueue on behalf of the
 * process. The actual value is determined by the system, but the upper
 * limit is specified as SOMAXCONN in <sys/socket.h>. The particular maximum
 * depends on the implementation of each protocol. For TCP, the default is
 * 128.
 * Once the queue is full, the system will reject additional connect
 * requests, so the backlog value must be chosen based on the expected load
 * of the server and the amount of processing it must do to accept a connect
 * request and start the service.
 *
 * Once a server has called listen(), the socket used can receive connect
 * requests. We use the accept() function to retrieve a connect request and
 * convert that into a connection.
 * int accept(int sockfd, struct sockaddr *addr, socklen_t *len);
 *      Returns: file (socket) descriptor if OK, -1 on error
 * The file descriptor returned by accept() is a socket descriptor that is
 * connected to the client that called connect(). This new socket descriptor
 * has the same socket type and address family as the original socket
 * (sockfd). The original socket passed to accept() is not associated with
 * the connection, but instead remains available to receive additional
 * connect requests.
 *
 * If we don't care about the client's identity, we can set the addr and
 * len parameters to NULL. Otherwise, before calling accept(), we need to
 * set the addr parameter to a buffer large enough to hold the address and
 * set the integer pointed to by len to the size of the buffer. On return,
 * accept() will fill in the client's address in the buffer and update the
 * integer pointed to by len to reflect the size of the address.
 *
 * If no connect requests are pending, accept() will block until one arrives
 * If sockfd is in nonblocking mode, accept() will return -1 and set errno
 * to either EAGAIN or EWOULDBLOCK. Linux中,EAGAIN和EWOULDBLOCK是一样的.
 *
 * If a server calls accept() and no connect request is present, the server
 * will block until one arrivers. ALternatively, a server can use either
 * poll() or select() to wait for a connect request to arrive. In this case,
 * a socket with pending connect requests will appear to be readable.
 */
int main(void)
{
    int sockfd, connfd;
    struct sockaddr_in sin, cli;
    struct sockaddr_in client;
    socklen_t clientlen, socklen;
    char buf[INET_ADDRSTRLEN];

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9527);

    printf("------------: server: address: 127.0.0.1\n");
    printf("------------: server: network port: %#x, host port: %#x\n",
            htons(9527), 9527);

    if (inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr) < 0) {
        perror("inet_pton, AF_INET, 127.0.0.1 error");
        return 1;
    }

    if ((sockfd = initserver(SOCK_STREAM, (struct sockaddr *)&sin, 
                    sizeof(sin), 10)) < 0) {
        perror("initserver: error");
        return 1;
    }

    /* The addrlen argument (指accept()函数的第三个参数) is a value-result
     * argument: the caller must initialize it to contain the size (in
     * bytes) of the structure pointed to by addr; on return it will contain
     * the actual size of the peer address.
     * 刚开始写的时候,没有写下面为clientlen赋值的语句,执行的时候,下面打印的
     * client 结构体相关的值都是0,似乎没有获取到客户端的地址,但是后面的
     * getpeername()函数是能获取到客户端地址的,打印出来的结果也正确.后来,加
     * 了下面为clientlen赋值的语句后,打印出来的client结构体的值就是正确的,
     * 可见accpet()函数的第三个参数一定要先赋值才能正确获取到客户端地址.
     */
    clientlen = sizeof(client);
    if ((connfd=accept(sockfd,(struct sockaddr *)&client,&clientlen))<0) {
        perror("accept error");
        return 1;
    }
    printf("------------: accept return: connfd: %d\n", connfd);

    if (inet_ntop(AF_INET,&client.sin_addr.s_addr,buf,sizeof(buf)) < 0) {
        perror("inet_ntop AF_INET error");
        return 1;
    }

    printf("------------: client: address: %s\n", buf);
    printf("------------: client: network port: %#x, host port: %#x\n",
            client.sin_port, ntohs(client.sin_port));

    socklen = sizeof(cli);
    if (getpeername(connfd, (struct sockaddr *)&cli, &socklen) < 0) {
        perror("getpeername error");
        return 1;
    }

    if (inet_ntop(AF_INET, &cli.sin_addr.s_addr, buf, sizeof(buf)) < 0) {
        perror("inet_ntop error");
        return 1;
    }
    printf("------------: getpeer name:client: address: %s\n", buf);
    printf("------------: getpeer name:client: network port: %#x, "
            "host port: %#x\n", cli.sin_port, ntohs(cli.sin_port));

    return 0;
}
