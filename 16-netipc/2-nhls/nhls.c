#include <stdio.h>
#include <stdint.h>     /* uint32_t, uint16_t, etc. */
#include <arpa/inet.h>

void printhex(void *pvalue, int size)
{
    char *p;
    int i;

    p = pvalue;
    for (i = 0; i < size; ++i) {
        printf("p[%d]: 0x%X\n", i, p[i] & 0xFF);
    }
}

/* Identifying the process has two components. The machine's network address
 * helps us identify the computer on the network we wish to contact, and the
 * service helps us identify the particular process on the computer.
 *
 * 大端 Big-endian: 低地址存放最高有效位(MSB),即高位字节排放在内存的低地址
 * 端,低位字节排放在内存的高地址端.
 * 小端 Lattle-endian: 低地址存放最低有效位 (LSB),即低位字节排放在内存的
 * 低地址端,高位字节排放在内存的高地址端.
 *
 * TCP/IP 网络协议指定网络中传输数据时,字节序是大端类型 (big-endian).
 * With TCP/IP, addresses are presented in network byte order, so
 * applications sometimes need to translate them between the processor's
 * byte order and the network byte order.
 * Four common functions are provided to convert between the processor
 * byte order and the network byte order for TCP/IP applications.
 * #include <arpa/inet.h>
 * uint32_t htonl(uint32_t hostint32);
 *  Returns: 32-bit integer in network byte order
 * uint16_t htons(uint16_t hostint16);
 *  Returns: 16-bit integer in network byte order
 * uint32_t ntohl(uint32_t netint32);
 *  Returns: 32-bit integer in host byte order
 * uint16_t ntohs(uint16_t netint16);
 *  Returns: 16-bit integer in host byte order
 *
 * The h is for "host" byte order, and the n is for "network" byte order.
 * The l is for "long" (i.e., 4-byte) integer, and the s is for "short"
 * (i.e., 2-byte) integer. These four functions are defined in <arpa/inet.h>
 * uint32_t, uint16_t 是定义在 <stdint.h> 中
 */
int main(void)
{
    uint32_t hostlong = 0xABCD1234;
    uint16_t hostshort = 0xA1B2;
    uint32_t netlong;
    uint16_t netshort;

    /* 打印本机字节序下,0xABCD1234 在内存中的存放顺序 */
    printf("host byte order: 0xABCD1234: \n");
    printhex(&hostlong, sizeof(hostlong));
    
    /* 将 0xABCD1234 转换为网络字节序后,再逐字节打印转换后的结果 */
    netlong = htonl(hostlong);
    printf("network byte order: 0xABCD1234: \n");
    printhex(&netlong, sizeof(netlong));

    /* 打印本机字节序下,0xA1B2 在内存中的存放顺序 */
    printf("host byte order: 0xA1B2: \n");
    printhex(&hostshort, sizeof(hostshort));

    /* 将 0xA1B2 转换为网络字节序后,再逐字节打印转换后的结果 */
    netshort = htons(hostshort);
    printf("network byte order: 0xA1B2: \n");
    printhex(&hostshort, sizeof(hostshort));

    /* 将网络字节序的 0x4321ABCD 转换为本机字节序,并逐字节打印出来 */
    netlong = 0x4321ABCD;
    hostlong = ntohl(netlong);
    printf("network: 0x4321ABCD --> host: 0x%X\n", hostlong);
    printhex(&hostlong, sizeof(hostlong));
  
    /* 将网络字节序的 0x4E2D 转换为本机字节序,并逐字节打印出来 */
    netshort = 0x4E2D;
    hostshort = ntohs(netshort);
    printf("network: 0x4E2D --> host: 0x%X\n", hostshort);
    printhex(&hostshort, sizeof(hostshort));

    return 0;
}
