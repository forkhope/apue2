#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

/* 打印 struct netent 结构体的内容 */
void printnetent(struct netent *ent)
{
    char **p;

    printf("network: n_name: %s\n", ent->n_name);

    printf("network: n_aliases:\n");
    for (p = ent->n_aliases; *p != NULL; ++p)
        printf("\t: %p\n", *p);
    printf("network: n_addrtype: %d,\t", ent->n_addrtype);
    if (ent->n_addrtype == AF_INET)
        printf("AF_INET\n");
    else if (ent->n_addrtype == AF_INET6)
        printf("AF_INET6\n");
    else
        printf("UNKNOWN TYPE\n");

    /* 注意, n_net 成员中保存的是本地字节序的值,不要调用inet_ntop()函数
     * 来做转换.这个转换结果打印出来是反的,例如本来是127.0.0.1,打印出来
     * 就会是 0.0.0.127
     */
    printf("network: network number: n_net: 0x%X\n", ent->n_net);
    printf("network: n_net to presentation: %d.%d.%d.%d\n",
            (ent->n_net>>24) & 0xFF, (ent->n_net>>16) & 0xFF,
            (ent->n_net>>8) & 0xFF, ent->n_net & 0xFF);
}

/* We can get network names and numbers with a similar set of interfaces.
 * #include <netdb.h>
 * void endnetent(void);
 * struct netent *getnetbyaddr(uint32_t net, int type);
 * struct netent *getnetbyname(const char *name);
 * struct netent *getentent(void);
 * void setnetent(int stayopen);
 *
 * These functions shall retrieve information about networks.
 *
 * The setnetent() function shall open and rewind the database. If the
 * stayopen argument is non-zero, the connection to the net database shall
 * not be closed after each call to getnetent().
 * 让指向下一项的指针指向数据库的第一项,后面再调用getnetent()将会从头开始
 * 读取数据库中的内容.
 *
 * The getnetent() functio shall read the next entry of the database,
 * opening and closing a connection to the database as necessary.
 *
 * The getnetbyaddr() function shall search the database from the begining,
 * and find the first entry for which the address family specified by type
 * matches the n_addrtype member and the network number net matches the
 * n_net member, opening and closing a connection to the database as
 * necessary. The net argument shall be the network number in host byte
 * order. 根据网络地址(network number)获取数据库中对应的项.
 *
 * The getnetbyname() function shall search the database from the begining
 * and find the first entry for which the network name specified by name
 * matches the n_name member, opening and closing a connection to the
 * database as necessary.
 *
 * The endnetent() function shall close the database, releasing any open
 * file descriptor.
 *
 * These functions need not be thread-safe.
 *
 * Upon successful completion, getnetbyaddr(), getnetbyname(), and
 * getnetent() shall return a pointer to a netent structure if the
 * requested entry was found, and a null pointer if the end of the database
 * was reached or the requested entry was not found. Otherwise, a null
 * pointer shall be returned.
 *
 * No errors are defined.
 *
 * Linux 中, struct netent 结构体定义如下:
 * struct netent {
 *      char    *n_name;     // official network name
 *      char   **n_aliases;  // alias list, terminated by NULL
 *      int      n_addrtype; // type of the network number, always AF_INET
 *      uint32_t n_net;      // the network number in host byte order
 * };
 *
 * 关于这些描述中提到的 "network number",指的是应该是IP地址的网络部分.
 * An IP address in two parts: network number portion and host number
 * portion. The highest order octec (most significant eight bits) in an
 * address was designated as the network number and the remaining bits
 * were called the rest field or host identifier and were used for host
 * numbering within a network.
 */
int main(void)
{
    struct netent *nets;
    int i;

    while ((nets = getnetent()) != NULL) {
        printnetent(nets);        
        printf("============ next network entry ========\n");
    }
    endnetent();
    
    /* 这个 "loopback" name 可以通过上面的打印得到,也可以查看/etc/networks
     * 文件得到,该文件的第一列就是网络名字,第二列是IP地址.
     */
    printf("~~~~~~~~~~~~~~~~~ begin getnetbyname ~~~~~~~~~~~~\n");
    if ((nets = getnetbyname("loopback")) != NULL) {
        printf("getnetbyname: loopback:\n");
        printnetent(nets);
    }
    endnetent();
    printf("~~~~~~~~~~~~~~~~~ end getnetbyname ~~~~~~~~~~~~\n");

    printf("~~~~~~~~~~~~~~~~~ begin getnetbyname ~~~~~~~~~~~~\n");
    /* 这个 0x7F000000 是根据上面的打印结果得到的. */
    nets = getnetbyaddr(0x7F000000, AF_INET);
    if (nets != NULL) {
        printf("getnetbyaddr net: 0x7F000000:\n");
        printnetent(nets);
    }
    printf("~~~~~~~~~~~~~~~~~ end getnetbyname ~~~~~~~~~~~~\n");
    endnetent();

    printf("--------------------------------------\n");
    nets = getnetent();
    if (nets != NULL) {
        printf("getnetbyaddr net: 127:\n");
        printnetent(nets);
    }
    setnetent(0);   /* 回滚数据库中指向下一项的指针到数据库开头 */

    printf("----------now, call the setnetent() function\n");
    for (i = 0; i < 2; ++i) {
        if ((nets = getnetent()) != NULL)
            printnetent(nets);
    }

    return 0;
}
