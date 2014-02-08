#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>

/* Returns a full-duplex "stream" pipe (a UNIX domain socket)
 * with the two file descriptors returned in fd[0] and fd[1].
 * 它返回的两个文件描述符是全双工的,即fd[0]即可读又可写,fd[1]
 * 也是即可读又可写.写入fd[0]的数据,从fd[1]读出;写入fd[1]的数
 * 据从fd[0]读出.
 */
int s_pipe(int fd[2])
{
    return socketpair(AF_UNIX, SOCK_STREAM, 0, fd);
}

/* UNIX domain sockets are used to communicate with processes running on
 * the same machine. Although Internet domain sockets can be used for this
 * same purpose, UNIX domain sockets are more efficient. UNIX domain sockets
 * only copy data; they have no protocol processing to perform, no network
 * headers to add or remove, no checksums to calcute, no sequence numbers
 * to generate, and no acknowledgements to send.
 *
 * UNIX domain sockets provide both stream and datagram interfaces. The UNIX
 * domain datagram service is reliable, however. Messages are neither lost
 * nor delivered out of order. UNIX domain sockets are like a cross between
 * sockets and pipes. You can use the network-oriented socket interfaces
 * with them, or you can use the socketpair() function to create a pair of
 * unnamed, connected, UNIX domain sockets.
 *
 * #include <sys/socket.h>
 * int socketpair(int domain, int type, int protocol, int sockfd[2]);
 *      Returns: 0 if OK, -1 on error
 * Although the interface is sufficiently general to allow socketpair() to
 * be used with arbitrary domains, operating systems typically provide
 * support only for the UNIX domain.
 */
int main(void)
{
    int fd[2], n;
    char buf[1024];

    if (s_pipe(fd) < 0) {
        perror("s_pipe: socketpair error");
        return 1;
    }

    printf("fd[0]: %d, fd[1]: %d\n", fd[0], fd[1]);

    printf("write fd[0], then read fd[1]\n");
    if (write(fd[0], "tian\n", 5) != 5) {
        perror("write error");
        return 1;
    }
    if ((n = read(fd[1], buf, 1024)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

    printf("write fd[1], then read fd[0]\n");
    if (write(fd[1], "xia\n", 4) != 4) {
        perror("write error");
        return 1;
    }
    if ((n = read(fd[0], buf, 1024)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }

    return 0;
}
