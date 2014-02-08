#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

/* Network host database functions
 * #include <netdb.h>
 * void endhostent(void);
 * struct hostent *gethostent(void):
 *          Returns: pointer if OK, NULL on error
 * void sethostent(int stayopen);
 * 
 * These functions shall retrieve information about hosts.
 * 
 * The sethostent() function shall open a connection to the database and set
 * the next entry for retrieval to the first entry in the database. If the
 * stayopen argument is non-zero, the connection shall not be closed by a
 * call to gethostent(), and the implementation may maintain an open file
 * descriptor.
 * 当调用 gethostent() 时,会向前读取下一项的内容.而调用 sethostent() 函数会
 * 与数据库建立一个连接,并让原来指向下一项的指针指向数据库第一项,也就是此时
 * 再调用 gethostent() 的话,将会从头开始再读取一遍.
 *
 * The gethostent() function shall read the next entry in the database,
 * opening and closing a connection to the database as necessary.
 * Entries shall be returned in hostent structures.
 * 每调用一次 gethostent() 函数,就返回数据库中的下一个项的指针,如果已经读完
 * 整个数据库,则返回NULL.即该函数要放到循环里面来调用,然后判断返回的指针是
 * 否等于NULL, 这个调用方法和 fgets() 函数相似.
 *
 * The endhostent() function shall close the connection to the database,
 * releasing any open file descriptor.
 *
 * These functions need not be thread-safe.
 *
 * Upon successful completion, the gethostent() function shall return a
 * pointer to a hostent structure if the requested entry was found, and a
 * null pointer if the end of the database was reached or the requested
 * entry was not found.
 * 
 * No errors are defined for endhostent(), gethostent(), and sethostent().
 *
 * Linux 中, hostent 结构体的定义如下:
 * struct hostent {
 *      char  *h_name;      // official name of host
 *      char **h_aliases;   // alias list, terminated by a NULL pointer.
 *      int    h_addrtype;  // host address type, always AF_INET
 *                          // or AF_INET6 at present;
 *      int    h_length;    // length of address
 *      char **h_addr_list; // list of address, terminated by a NULL pointer
 *                    // The addresses returned are in network byte order.
 * };
 *
 * 关于这些描述中提到的 "host",指的是应该是IP地址的主机部分.
 * An IP address in two parts: network number portion and host number
 * portion. The highest order octec (most significant eight bits) in an
 * address was designated as the network number and the remaining bits
 * were called the rest field or host identifier and were used for host
 * numbering within a network.
 *
 * 这个程序的执行结果和 /etc/hosts 里面的结果很类似,但是 /etc/hosts 最后面
 * 的ip6-* 部分地址没有打印出来.
 */
int main(void)
{
    struct hostent *host;
    char **p, buf[INET6_ADDRSTRLEN];
    int i;

    /* 全部遍历数据库中的host项,并打印里面的内容 */
    while ((host = gethostent()) != NULL) {
        printf("host: h_name: %s\n", host->h_name);
        printf("host: h_aliases:\n");

        /* 注意,下面是判断 *p 是否等于 NULL,而不是判断 p 是否等于 NULL,因为
         * p 不可能等于NULL,执行 p = host->h_aliases; 后, p 是一个地址,后面
         * 一直执行 ++p, 不断的往上加, p 不会等于 NULL. 实际上要判断的是, p
         * 指向的地址中保存的值是否为 NULL,如果不是NULL,才是有效值.
         */
        for (p = host->h_aliases; *p != NULL; ++p)
            printf("\t%p\n", *p);

        printf("host: h_addrtype: %d", host->h_addrtype);
        if (host->h_addrtype == AF_INET)
            printf(",\tAF_INET: %d\n", AF_INET);
        else if (host->h_addrtype == AF_INET)
            printf(",\tAF_INET6: %d\n", AF_INET6);
        else
            printf(",\tUNKNOWN TYPE\n");

        printf("host: h_length: %d\n", host->h_length);
        printf("host: h_addr_list:\n");
        for (p = host->h_addr_list; *p != NULL; ++p) {
            printf("\t%p\n", *p);   /* 打印出来的是网络字节序的地址 */
            if (inet_ntop(host->h_addrtype, *p, buf, INET6_ADDRSTRLEN) < 0)
                printf("inet_ntop, error: %m\n");
            /* 转换为可读格式的 IP 地址 */
            printf("\tinet_ntop: presentation: %s\n", buf);
        }
        printf("=============== next host ent ============\n");
    }
    endhostent();

    /* 先遍历前面两项,这样数据库中指向下一项的指针向前移动两项 */
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    for (i = 0; i < 2; ++i)
        if ((host = gethostent()) != NULL) {
            printf("2222: host: h_name: %s\n", host->h_name);
        }

    /* 执行 sethostent() 后,数据库中指向下一项的指针会被恢复,重新指向
     * 数据库的第一项,所以后面再遍历前面两项时,会打印出和上面相同的内容.
     */
    printf("-----------Call the sethostent() function\n");
    sethostent(0);
    
    printf("~~~~~Again~~~~~~~~~~~~~~~~~~~~~~~\n");
    for (i = 0; i < 2; ++i)
        if ((host = gethostent()) != NULL) {
            printf("2222: host: h_name: %s\n", host->h_name);
        }

    endhostent();
    return 0;
}
