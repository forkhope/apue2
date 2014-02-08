#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>

void printservent(struct servent *ent)
{
    char **p;

    printf("getservent: s_name: %s\n", ent->s_name);
    printf("getservent: s_aliases:\n");
    for (p = ent->s_aliases; *p != NULL; ++p)
       printf("\t%s\n", *p);
    /* s_port 成员保存的是网络字节序的端口值. 注意,下面一定要调用
     * ntohs()函数来做转换,由于 struct servent 结构体中, s_port 是int型,
     * 容易产生迷惑,觉得要调用ntohl()函数来转换,但是调用ntohl()函数来
     * 转换会产生错误的结果, ntohs(0x5000)得到 80, ntohl(0x5000)得到
     * 5242880,两者得到的结果并不同.
     */
    printf("getservent: s_port: network order: %#x\n", ent->s_port);
    printf("getservent: s_port: host order: %u\n", ntohs(ent->s_port));
    printf("getservent: s_proto: %s\n", ent->s_proto);
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}

/* Services are represented by the port number portion of the address. Each
 * service is offered on a unique, well-known port number. We can map a
 * service name to a port number with getservbyname(), map a port number to
 * a service name with getservbyport(), or scan the services database
 * sequentially with getservent().
 *
 * #include <netdb.h>
 * void endservent(void);
 * struct servent *getservbyname(const char *name, const char *proto);
 * struct servent *getservbyport(int port, const char *proto);
 * struct servent *getservent(void):
 * void setservent(int stayopen);
 *
 * These functions shall retrieve information about network services.
 *
 * The setservent() function shall open a connection to the database, and
 * set the next entry to the first entry. If the stayopen argument is non-
 * zero, the net database shall not be closed after each call to the
 * getservent() function (either directly, or indirectly through one of the
 * other getserv*() functions).
 *
 * The getservent() function shall read the next entry of the database,
 * opending and closing a connection to the database as necessary.
 *
 * The getservbyname() function shall search the database from the beginning
 * and find the first entry for which the service name specified by name
 * matches the s_name member and the protocol name specified by proto
 * matches the s_proto mumber, opening and closing a connection to the
 * database necessary. If proto is a null pointer, any value of the s_proto
 * member shall be matched. 当 proto 是NULL指针时,将匹配任意的协议名,但是
 * getservbyname()函数还是只返回第一项匹配的指针,不会返回多项.这里的"匹配
 * 任意协议名"应该是指不判断协议名.
 *
 * The getservbyport() function shall search the database from the beginning
 * and find the first entry for which the port specified by port matches the
 * s_port member and the protocol name specified by proto matches the
 * s_proto member, opening and closing a connection to the database as
 * necessary. If proto is a null pointer, any value of the s_proto member
 * shall be matched. The port argument shall be a value obtained by
 * converting a uint16_t in network byte order to int. 即 port 参数的值
 * 是网络字节序的,经过测试,要调用htons()函数来转换得到网络字节序的值,不能
 * 使用htonl()函数, htonl(80) 得到的值是0x50000000, htons(80) 得到的值是
 * 0x5000, 两者的结果是不同的.
 *
 * The endservent() function shall close the database, releasing any open
 * file descriptor.
 *
 * These functions need not be thread-safe.
 *
 * Upon successful completion, getservbyname(), getservbyport(), and
 * getservent() return a pointer to a servent structure if the requested
 * entry was found, and a null pointer if the end of the database was
 * reached or the requested entry was not found. Otherwise, a null pointer
 * is returned. No errors are defined.
 *
 * Linux 中, struct servent 结构体定义如下:
 * struct servent {
 *      char    *s_name;    // official service name
 *      char   **s_aliases; // alias list, terminated by NULL
 *      int      s_port;    // The port number for the service given
 *                          // in network byte order
 *      char     *s_proto;  // protocol to use with this service
 * };
 *
 * 查看执行结果,发现输出的服务名和 /etc/services 文件里面的服务名是一样的.
 */
int main(void)
{
    struct servent *serv;
    int i, port;

    while ((serv = getservent()) != NULL)
        printservent(serv);
    endservent();

    printf("========= getservbyname: name --> echo, proto --> tcp\n");
    if ((serv = getservbyname("echo", "tcp")) != NULL)
        printservent(serv);

    /* 虽然 echo 服务可以使用tcp协议,也可以使用udp协议,但是下面的语句还是
     * 只会返回其中一项的值,指定getservbyname()函数的第二个参数为NULL,并
     * 不会两个echo服务 (使用tcp协议的echo和使用udp协议的echo).
     */
    printf("========= getservbyname: name --> echo, proto --> NULL\n");
    if ((serv = getservbyname("echo", NULL)) != NULL)
        printservent(serv);

    printf("========= getservbyprot: port --> 80, proto --> tcp\n");
    port = htons(80);
    printf("port: host 80 --> network is: %#x\n", port);
    if ((serv = getservbyport(port, "tcp")) != NULL)
        printservent(serv);

    printf("========= setservent() =========\n");
    if ((serv = getservent()) != NULL)
        printservent(serv);
    setservent(0);

    printf("========= after setservent() =========\n");
    for (i = 0; i < 2; ++i)
        if ((serv = getservent()) != NULL)
            printservent(serv);
    endservent();

    return 0;
}
