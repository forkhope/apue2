#include <stdio.h>
#include <string.h>
#include <netinet/in.h>     /* 包含 struct sockaddr_in 结构体 */
#include <sys/socket.h>     /* socket(), bind(), getsockname() */
#include <arpa/inet.h>      /* inet_ntop() */

/* We use the bind() function to associate an address with a socket.
 * #include <sys/socket.h>
 * int bind(int sockfd, const struct sockaddr *addr, socklen_t len);
 *      Returns: 0 if OK, -1 on error
 * When a socket is created with socket(), it exists in a name space (
 * address family) but has no address assigned to it. bind() assigns the
 * address specified to by addr to the socket referred to by the file
 * descriptor sockfd.
 * There are several restrictions on the address we can use:
 * 1. The address we specify must be valid for the machine on which the
 * process is running; we can't specify an address belonging to some other
 * machine.
 * 2. The address must match the format supported by the address family we
 * used to create the socket.
 * 3. The port number in the address cannot be less that 1024 unless the
 * process has the appropriate privilege (i.e., is the superuser).
 * 4. Usually, only one socket endpoint can be bound to a given address,
 * although some protocols allow duplicate bindings.
 *
 * For the Internet domain, if we specify the special IP address INADDR_ANY,
 * the socket endpoint will be bound to all the system'a network interfaces.
 * This means that we can receive packets from any of the network interface
 * cards installed in the system.
 *
 * We can use the getsockname() function to discover the address bound to
 * a socket.
 * #include <sys/socket.h>
 * int getsockname(int sockfd, struct sockaddr *addr, socklen_t *alenp);
 *      Returns: 0 if OK, -1 on error
 * Before calling getsockname(), we set alenp to point to an integer
 * containing the size of the sockaddr buffer. On return, the integer is set
 * to the size of the address returned. If the address won't fit in the
 * buffer provided, the address is silently truncated. If no address is
 * currently bound to the socket, the results are undefined.
 *
 * If the socket is connected to a peer, we can find out the peer's address
 * by calling the getpeername() function.
 * int getpeername(int sockfd, struct sockaddr *addr, socklen_t alenp);
 *          Returns: 0 if OK, -1 on error
 * Other than returning the peer's address, the getpeername() function is
 * identical to the getsockname() function.
 */
int main(void)
{
    int sockfd;
    struct sockaddr_in sin, sockname;
    socklen_t addrlen;
    char buf[INET_ADDRSTRLEN];

    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket AF_INET, SOCK_STREAM error");
        return 1;
    }
    printf("socket SUCCESS: %d\n", sockfd);

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(9527);
    sin.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr *)&sin, sizeof(sin))< 0) {
        perror("bind error");
        return 1;
    }
    printf("bind SUCCESS\n");


    addrlen = sizeof(sockname); /* 设置 addrlen 为传入的地址结构大小 */
    if (getsockname(sockfd, (struct sockaddr *)&sockname, &addrlen) < 0) {
        perror("getsockname: error");
        return 1;
    }

    printf("getsockname: sin_family: %d, ", sockname.sin_family);
    switch (sockname.sin_family) {
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

    printf("getsockname: sin_addr: network: %#X, ", 
            sockname.sin_addr.s_addr);
    if (inet_ntop(AF_INET, &sockname.sin_addr.s_addr, 
                buf, INET_ADDRSTRLEN) == NULL) {
        perror("inet_ntop: error");
        return 1;
    }
    printf("host: %s\n", buf);

    printf("getsockname: sin_port: network: %#x, ", sockname.sin_port);
    printf("host hex: %#x, ", ntohs(sockname.sin_port));
    printf("host decimal: %u\n", ntohs(sockname.sin_port));

    return 0;
}
