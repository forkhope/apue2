#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <sys/socket.h> /* AF_INET, AF_INET6 */
#include <arpa/inet.h>  /* inet_ntop(), inet_pton */

/* So that addresses with different formats can be passed to the socket
 * functions, the addresses are cast to a generic sockaddr address structure
 * struct sockaddr {
 *      sa_family_t sa_family;  // address family
 *      char sa_data[];         // variable-length address
 * };
 * Implementations are free to add additional members and define a size for
 * the sa_data member.
 *
 * Internet addresses are defined in <netinet/in.h>. In the IPv4 Internet
 * domain (AF_INET), a socket address is represented by a sockaddr_in
 * structure:
 * struct in_addr {
 *      in_addr_t   s_addr;     // IPv4 address
 * };
 * struct sockaddr_in {
 *      sa_family_t     sin_family; // address family
 *      in_port_t       sin_port;   // port number
 *      struct in_addr  sin_addr;   // IPv4 address
 * };
 * The in_prot_t data type is defined to be a uint16_t. The in_addt_t data
 * type is defined to be a uint32_t.
 *
 * In contrast to the AF_INET domain, the IPv6 Internet domain (AF_INET6)
 * socket address is represented by a sockaddr_in6 structure:
 * struct in6_addr {
 *      uint8_t     s6_addr[16];    // IPv6 address
 * };
 * struct sockaddr_in6 {
 *      sa_family_t     sin6_family;    // address family
 *      in_port_t       sin6_port;      // port number
 *      uint32_t        sin6_flowinfo;  // traffic class and flow info
 *      struct in6_addr sin6_addr;      // IPv6 address
 *      uint32_t        sin6_scope_id;  // set of interfaces for scope
 * };
 *
 * It is sometimes necessary to print an address in a format that is
 * understandable by a person instead of a computer. The BSD networking
 * software included the inet_addr() and inet_ntoa() functions to convert
 * between the binary address format and a string in dotted-decimal notation
 * (a.b.c.d). There functions, however, work only with IPv4 addresses. Two
 * new functions--inet_ntop() and inet_pton()--support similar functionality
 * and work with both IPv4 and IPv6 addresses.
 * #include <arpa/inet.h>
 * const char *inet_ntop(int domain, const void *addr, 
 *          char *str, socklen_t size);
 *          Returns: pointer to address string on success, NULL on error
 * int inet_pton(int domain, const char *str, void *addr);
 *          Returns: 1 on success, 0 if format is invalid, or -1 on error
 * The inet_ntop() function converts a binary address in network byte order
 * into a text string: inet_pton() converts a text string into a binary
 * address in network byte order. Only two domain values are supported:
 * AF_INET and AF_INET6.
 *
 * For inet_ntop(), the size parameter specifies the size of the buffer
 * (str) to hold the text string. Two constants are defined to make our job
 * easier: INET_ADDRSTRLEN is large enough to hold a text string represent-
 * ing an IPv4 address, and INET6_ADDRSTRLEN is large enough to hold a text
 * string representing an IPv6 address. For inet_pton(), the addr buffer
 * needs to be large enough to hold a 32-bit address if domain is AF_INET
 * or large enough to hold a 128-bit address if domain is AF_INET6;
 */
int main(void)
{
    char inet4_p[INET_ADDRSTRLEN] = "192.168.1.1";
    uint32_t inet4_n;
    char inet6_p[INET6_ADDRSTRLEN] = "0:0:0:0:0:FFFF:204.152.189.116";
    struct in6_addr inet6_n;
    int i;
    char *p;

    /* 转换 IPv4 地址 192.168.1.1 为网络字节序 */
    if (inet_pton(AF_INET, inet4_p, &inet4_n) < 0) {
        printf("inet_pton, AF_INET error: %m\n");
        return 1;
    }
    printf("192.168.1.1 to hex: ");
    printf("%#X.", 192);
    printf("%#X.", 168);
    printf("%#X.", 1);
    printf("%#X\n", 1);
    printf("inet_pton: 192.168.1.1 --> 0x%08X\n", inet4_n);

    /* 将网络字节序的 0x21A34D0A 转换为 x.x.x.x 的形式  */
    inet4_n = 0x21A34D0A;
    if (inet_ntop(AF_INET, &inet4_n, inet4_p, INET_ADDRSTRLEN) == NULL) {
        printf("inet_ntop: AF_INET error: %m\n");
        return 1;
    }
    printf("(network)0x21A34D0A --> (host)0x0A4DA321 to decimal: ");
    printf("%d.", 0x0A);
    printf("%d.", 0x4D);
    printf("%d.", 0xA3);
    printf("%d\n", 0x21);
    printf("inet_ntop: 0x21A34D0A --> %s\n", inet4_p);

    /* 将 IPv6 地址转换为网络字节序 */
    if (inet_pton(AF_INET6, inet6_p, &inet6_n) < 0) {
        printf("inet_pton: AF_INET6 error: %m\n");
        return 1;
    }
    printf("inet_pton: %s --> \n\t\t", inet6_p);
    p = (char *)&inet6_n;
    for (i = 0; i < sizeof(inet6_n); ++i) {
        printf("%#X ", p[i] & 0xFF);
    }
    putchar('\n');

    /* 将刚才转换得到的网络字节序的IPv6地址再转换为可读的形式 */
    memset(inet6_p, 0, sizeof(inet6_p));
    if (inet_ntop(AF_INET6, &inet6_n, inet6_p, INET6_ADDRSTRLEN) == NULL) {
        printf("inet_ntop: AF_INET6 error: %m\n");
        return 1;
    }
    printf("inet_ntop: AF_INET6: %s\n", inet6_p);

    return 0;
}
