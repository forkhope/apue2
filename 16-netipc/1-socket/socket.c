#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>         /* for IPPROTO_TCP */

/* A socket is an abstraction of a communication endpoint. Just as they
 * would use file descriptors to access a file, applications use socket
 * descriptors to access sockets. Socket descriptors are implemented as
 * file descriptors in the UNIX System. Indedd, many of the function that
 * deal with file descriptors, such as read() and write(), will work a
 * socket descriptor.
 * To create a socket, we call the socket() function.
 * #include <sys/socket.h>
 * int socket(int domain, int type, int protocol);
 *      Returns: socket descriptor if OK, -1 on error
 * The domain argument determines the nature of the communication, including
 * the address format. The constants start with AF_ (for address family)
 * because each domain has its own format for representing an address.
 * (1) AF_INET: IPv4 Internet domain. (2) AF_INET6 IPv6 Internet domain
 * (3) AF_UNIX: UNIX domain           (4) AF_UNSPEC: unspecified
 *
 * The type argument determines the type of the socket, which further
 * determines the communication characteristics.
 * (1) SOCK_DGRAM: fixed_length, connectionless, unreliable messages
 * (2) SOCK_RAW: datagram interface to IP
 * (3) SOCK_SEQPACKET: fixed-length, sequenced, reliable,
 * connection-oriented messages
 * (4) SOCK_STREAM: sequenced, reliable, bidirectional,
 * connection-oriented byte streams
 *
 * The protocol argument is usually zero, to select the default protocol
 * for the given domain and socket type. When multiple protocols are
 * supported for the same domain and socket type, we can use the protocol
 * argument to select a particular protocol. The default protocol for a
 * SOCK_STREAM socket in the AF_INET communication domain is TCP. The
 * default protocol for a SOCK_DGRAM socket in the AF_INET communication
 * domain is UDP (User Datagram Protocol).
 *
 * Communication on a socket is bidirectional. We can disable I/O on a
 * socket with the shudown() function.
 * #include <sys/socket.h>
 * int shutdown(int sockfd, int how);
 *      Returns: 0 if OK, -1 on error
 * If how is SHUT_RD, then reading from the socket is disabled. If how is
 * SHUT_WR, then we can't user the socket for transmitting data. We can
 * use SHUT_RDWR to disable both data transmission and reception.
 */
int main(void)
{
    int intcp, inudp, in6tcp, in6udp, unixtcp, unixudp, sockraw;

    if ((intcp = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("socket AF_INET & SOCK_STREAM error: %m\n");
        return 1;
    }
    printf("intcp: %d\n", intcp);

    if ((inudp = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf("socket AF_INET & SOCK_DGRAM error: %m\n");
        return 1;
    }
    printf("inudp: %d\n", inudp);

    if ((in6tcp = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
        printf("socket AF_INET6 & SOCK_STREAM error: %m\n");
        return 1;
    }
    printf("in6tcp: %d\n", in6tcp);

    if ((in6udp = socket(AF_INET6, SOCK_DGRAM, 0)) < 0) {
        printf("socket AF_INET6 & SOCK_DGRAM error: %m\n");
        return 1;
    }
    printf("in6udp: %d\n", in6udp);

    if ((unixtcp = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        printf("socket AF_UNIX & SOCK_STREAM error: %m\n");
        return 1;
    }
    printf("unixtcp: %d\n", unixtcp);

    if ((unixudp = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        printf("socket AF_UNIX & SOCK_DGRAM error: %m\n");
        return 1;
    }
    printf("unixudp: %d\n", unixudp);

    /* 经过测试发现, SOCK_RAW 类型需要指定明确的协议类型,否则报错,
     * 错误码是 EPROTONOSUPPORT.
     */
    if ((sockraw = socket(AF_INET, SOCK_RAW, 0)) < 0) {
        printf("strerror(EPROTONOSUPPORT): %s\n", 
                strerror(EPROTONOSUPPORT));
        printf("socket AF_INET & SOCK_RAW error: %m\n");
    }

    /* 使用 IPPROTO_TCP 执行协议类型为TCP,然后用SOCK_RAW创建这种类型
     * 的socket,执行时会提示没有权限.因为创建原始socket需要超级用户权限.
     * Superuser privileges are required to create a raw socket to prevent
     * malicious applications from creating packets that might bypass
     * established security mechanisms.
     * 所以用普通用户来执行时报错,错误码是EPERM,但奇怪的是, man socket手册
     * 中,没有提到该错误码,按照man手册里面的描述,权限不足应该返回EACCES.
     */
    if ((sockraw = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
        printf("strerror(EPERM): %s\n", strerror(EPERM));
        printf("socket AF_INET & SOCK_RAW error: %m\n");
    }

    shutdown(intcp, SHUT_RDWR);
    shutdown(inudp, SHUT_RD);
    shutdown(in6tcp, SHUT_WR);
    close(in6udp);
    close(unixtcp);
    close(unixudp);
    return 0;
}
