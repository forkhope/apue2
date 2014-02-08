#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <netdb.h>

#define BUFLEN 128
#define TIMEOUT 20

void sigalarm(int signum)
{
}

void print_sockname(struct sockaddr_in addr)
{
    char buf[INET_ADDRSTRLEN];

    switch (addr.sin_family) {
        case AF_INET:
            printf("AF_INET\n");
            break;
        case AF_INET6:
            printf("AF_INET6\n");
            break;
        default:
            printf("**UNKNOWN**\n");
            break;
    }

    printf("addr: sin_addr: network: %#x, ", addr.sin_addr.s_addr);
    if (inet_ntop(AF_INET, &addr.sin_addr.s_addr,
                buf, sizeof(buf)) == NULL) {
        perror("inet_ntop error");
        return;
    }
    printf("host: %s\n", buf);

    printf("addr: sin_port: network: %#x, ", addr.sin_port);
    printf("host hex: %#x, ", ntohs(addr.sin_port));
    printf("host decimal: %u\n", ntohs(addr.sin_port));
}

void print_uptime(int sockfd, struct sockaddr *addr, socklen_t addrlen)
{
    char buf[BUFLEN];
    int n;
    struct sockaddr_in sockname;
    socklen_t namelen;

    memset(&sockname, 0, sizeof(sockname));
    namelen = sizeof(sockname);
    if (getsockname(sockfd, (struct sockaddr *)&sockname, &namelen) < 0) {
        perror("getsockname error");
        return;
    }
    print_sockname(sockname);

    buf[0] = 0;
    if (sendto(sockfd, buf, 1, 0, addr, addrlen) < 0) {
        perror("sendto error");
        return;
    }

    printf("after call sendto(), getsockname: \n");
    memset(&sockname, 0, sizeof(sockname));
    namelen = sizeof(sockname);
    if (getsockname(sockfd, (struct sockaddr *)&sockname, &namelen) < 0) {
        perror("getsockname error");
        return;
    }
    /* 打印出 sockname 后,可以看到,执行sendto(),会为sockfd分配一个端口,但
     * 是地址还是默认的地址 0.0.0.0,后续对sockfd的操作,都会跟刚才分配的端口
     * 关联起来.如下面的recvfrom()函数并没有明确指定读取的是哪个端口的数据,
     * 但是它还是能正确读取到数据,而不会跟其他进程的recvfrom()函数冲突,原因
     * 就是其使用的参数sockfd已经关联到具体端口了.
     */
    print_sockname(sockname);
    printf("===========================================\n");

    alarm(TIMEOUT);
    if ((n = recvfrom(sockfd, buf, BUFLEN, 0, NULL, NULL)) < 0) {
        if (errno != EINTR)
            alarm(0);
        perror("recvfrom error");
        return;
    }
    alarm(0);
    write(STDOUT_FILENO, buf, n);
}

/* This is a version of the uptime client command that uses the datagram
 * socket interface. In this program, we use the alarm() function to avoid
 * blocking indefinitely in the call to recvfrom().
 * With the connection-oriented protocol, we needed to connect to the
 * server before exchanging data. The arrival of the connect request was
 * enough for the server to determine that it needed to provide service to
 * a client. But with the datagram-based protocol, we need a way to notify
 * the server that we want it to perform its service on our behalf. In this
 * example, we simply send the server a 1-byte message. The server will
 * receive it, get out address from the packet, and use this address to
 * transmit its response. If the server offered multiple services, we could
 * use this request message to indicate the service we want, but since the
 * server does only one thing, the content of the 1-byte message doesn't
 * matter. 这段话,说的是服务器怎么知道客户端的地址,来发送消息给客户端?在
 * 这个程序中,它是通过先发送数据给服务器,而服务器也恰好在接收客户端的数据,
 * 从而知道了客户端的地址,然后才发送数据给客户端.
 *
 * If the server isn't running, the client will block indefinitely in the
 * call to recvfrom(). With the connection-oriented example, the connect()
 * call will fail if the server isn't running. To avoid blocking
 * indefinitely, we set an alarm clock before calling recvfrom().
 */
int main(int argc, char *argv[])
{
    int sockfd, err;
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sigaction sa;

    if (argc != 2) {
        printf("usage: %s hostname\n", argv[0]);
        return 1;
    }

    sa.sa_handler = sigalarm;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGALRM, &sa, NULL) < 0) {
        perror("sigaction error");
        return 1;
    }

    hint.ai_flags = 0;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = socket(aip->ai_family, SOCK_DGRAM, 0)) >= 0) {
            print_uptime(sockfd, aip->ai_addr, aip->ai_addrlen);
            return 0;
        }
    }

    printf("can't contact %s: %s\n", argv[1], strerror(errno));
    return 1;
}
