#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h> /* for IPPROTO_TCP */

/* 打印额外的选项 */
void print_flags(struct addrinfo *aip)
{
    printf("flags:");
    if (aip->ai_flags == 0)
        printf(" 0");
    else {
        /* AI_PASSIVE: Socket address is intended to be bound for
         * listening.
         */
        if (aip->ai_flags & AI_PASSIVE)
            printf(" passive");

        /* AI_CANONNAME: Request a canonical name (as apposed to
         * an alias).
         */
        if (aip->ai_flags & AI_CANONNAME)
            printf(" conon");

        /* AI_NUMERICHOST: Return the host address in numeric format. */
        if (aip->ai_flags & AI_NUMERICHOST)
            printf(" numhost");

        /* AI_NUMERICSERV: Return the serivces as a port number. */
        if (aip->ai_flags & AI_NUMERICSERV)
            printf(" numserv");

        /* AI_V4MAPPED: If no IPv6 addresses are found, return IPv4
         * address mapped in IPv6 format.
         */
        if (aip->ai_flags & AI_V4MAPPED)
            printf(" v4mapped");

        /* AI_ALL: Look for both IPv4 and IPv6 addresses (used only
         * with AI_V4MAPPED).
         */
        if (aip->ai_flags & AI_ALL)
            printf(" all");
    }
}

/* 打印协议族的名称 */
void print_family(struct addrinfo *aip)
{
    printf(", family: ");
    switch (aip->ai_family) {
        case AF_INET:
            printf("inet");
            break;
        case AF_INET6:
            printf("inet6");
            break;
        case AF_UNIX:
            printf("unix");
            break;
        case AF_UNSPEC:
            printf("unspecified");
            break;
        default:
            printf("unknown family: %d\n", aip->ai_family);
            break;
    }
}

/* 打印socket类型 */
void print_type(struct addrinfo *aip)
{
    printf(", type: ");
    switch (aip->ai_socktype) {
        case SOCK_STREAM:
            printf("stream");
            break;
        case SOCK_DGRAM:
            printf("datagram");
            break;
        case SOCK_RAW:
            printf("raw");
            break;
        case SOCK_SEQPACKET:
            printf("seqpacket");
            break;
        default:
            printf("unknown type: %d\n", aip->ai_socktype);
            break;
    }
}

/* 打印socket对应的协议 */
void print_protocol(struct addrinfo *aip)
{
    printf(", protocol: ");
    switch (aip->ai_protocol) {
        case 0:
            printf("default");
            break;
        case IPPROTO_TCP:
            printf("tcp");
            break;
        case IPPROTO_UDP:
            printf("udp");
            break;
        case IPPROTO_RAW:
            printf("raw");
            break;
        default:
            printf("unknown protocol: %d\n", aip->ai_protocol);
            break;
    }
}

/* POSIX.1 defines several new functions to allow an application to map
 * from a host name and a service name to an address and vice versa.
 * These functions replace the older gethostbyname() and gethostbyaddr()
 * functions. The getaddrinfo() function allows us to map a host name and
 * a service name to an address.
 * #include <sys/socket.h>
 * #include <netdb.h>
 * int getaddrinfo(const char *host, const char *service,
 *      const struct *hint, struct addrinfo **res);
 *      Returns: 0 if OK, nonzero error code on error
 * void freeaddrinfo(struct addrinfo *ai);
 *
 * We need to provide the host name, the service name, or both. If we
 * provide only one name, the other should be a null pointer. The host
 * name can be either a node name or the host address in dotted-decimal
 * notation. 如 www.baidu.com, www.google.com 等
 * The getaddrinfo() function returns a linked list of addrinfo structures.
 * We can use freeaddrinfo() to free one or more of these structures,
 * depending on how many structures are linked together using the ai_next
 * field. The addrinfo structure is defined to include at least the
 * following members:
 * struct addrinfo {
 *      int     ai_flags;       // customize behavior
 *      int     ai_family;      // address family
 *      int     ai_socktype;    // socket type
 *      int     ai_protocol;    // protocol
 *      socklen_t ai_addrlen;   // length in bytes of address
 *      struct sockaddr *ai_addr;   // address
 *      char   *ai_canonname;   // canonical name of host
 *      struct addrinfo *ai_next;   // next in list
 * };
 *
 * We can supply an optional hint to select addresses that meet certain
 * criteria. The hint is a template used for filtering addresses and uses
 * only the ai_family, ai_flags, ai_protocol, and ai_socktype fields. The
 * remainning integer fields must be set to 0, and the pointer fields must
 * be null.
 *
 * If getaddrinfo() fails, we can't use perror() or strerror() to generate
 * an error message. Instead, we need to call gai_strerror() to convert
 * the error code returned into an error message.
 * #include <netdb.h>
 * const char *gai_strerror(int error);
 *  Returns: a pointer to a string describing the error
 *
 * getaddrinfo() 函数的host参数可以是本地的主机名(通过hostname命令来获取),
 * 也可以是网络上的域名,如www.baidu.com, www.google.com 等,此时就能获取
 * 到www.baidu.com对应的地址信息,如ip地址,服务的端口地址,协议类型等.
 *
 * The getnameinfo() function converts an address into a host name and
 * a service name.
 * #include <sys/socket.h>
 * #include <netdb.h>
 * int getnameinfo(const struct sockaddr *addr, socklen_t alen, char *host,
 *      socklen_t hostlen, char *service, socklen_t servlen,
 *      unsigned int flags);
 *          Returns: 0 if OK, nonzero on error
 * The socket address (addr) is translated into a host name and a service
 * name. If host is non-null, it points to a buffer hostlen bytes long
 * that will be used to return the host name. Similarly, if service is
 * non-null, it points to a buffer servlen bytes long that will be used to
 * return the service name.
 *
 * The flags argument gives us some control over how the translation is done
 */
int main(int argc, char *argv[])
{
    struct addrinfo *aliases, *aip;
    struct addrinfo hint;
    struct sockaddr_in *sinp;
    const char *addr;
    int err;
    char abuf[INET_ADDRSTRLEN];

    if (argc != 3) {
        printf("usage: %s  nodename servicename\n", argv[0]);
        return 1;
    }

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = 0;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], argv[2], &hint, &aliases)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }

    /* If multiple protocols provide the given service for the given host,
     * the program will print more than one entry.
     */
    for (aip = aliases; aip != NULL; aip = aip->ai_next) {
        print_flags(aip);
        print_family(aip);
        print_type(aip);
        print_protocol(aip);
        printf("\n\thost: %s", aip->ai_canonname?aip->ai_canonname:"-");
        if (aip->ai_family == AF_INET) {
            sinp = (struct sockaddr_in *)aip->ai_addr;
            /* inet_ntop() 函数返回 const char * 类型指针,所以 addr 也要
             * 定义为 const char * 类型,否则编译报警:
             * assignment discards qualifiers from pointer target type
             */
            addr = inet_ntop(AF_INET, &sinp->sin_addr, abuf,
                    INET_ADDRSTRLEN);
            printf(", address: %s", addr ? addr : "unknown");
            printf(", port: %d", ntohs(sinp->sin_port));
        }
        printf("\n");
    }

    freeaddrinfo(aliases);
    return 0;
}
