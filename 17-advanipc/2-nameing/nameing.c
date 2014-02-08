#include <stdio.h>
#include <stddef.h> /* offsetof() */
#include <sys/socket.h>
#include <sys/un.h> /* struct sockaddr_un */
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

/* Although the socketpair() function creates sockets that are connected
 * to each other, the individual sockets don't have names. This means that
 * they can't be addressed by unrelated processes.
 * Just as with Internet domain sockets, UNIX domain sockets can be named
 * and used to advertise services. The address format used with UNIX domain
 * sockets differs from Internet domain sockets, however.
 * An address for a UNIX domain socket is represented by a sockaddr_un
 * structure. On Linux, the sockaddr_un structure is defined in the header
 * <sys/un.h> as follows:
 * struct sockaddr_un {
 *      sa_family_t sun_family;     // AF_UNIX
 *      char        sun_path[108];  // pathname
 * };
 *
 * The sun_path member of the sockaddr_un structure contains a pathname.
 * When we bind an address to a UNIX domain socket, the system creates a
 * file of type S_IFSOCK with the same name.
 *
 * This file exists only as a means of advertising the socket name to
 * clients. The file can't be opened or otherwise used for communication
 * by applications.
 *
 * If the file already exists when we try to bind the same address, the
 * bind() request will fail. When we close the socket, this file is not
 * automatically removed, so we need to make sure that we unlink it before
 * our application exits.
 */
int main(void)
{
    int size, sockfd;
    struct sockaddr_un un;
    char *filename;

    filename = "foo.socket";
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, filename);
    if ((sockfd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        perror("socket error");
        return 1;
    }

    /* The way we determine the size of the address to bind is to determine
     * the offset of the sun_path member in the sockaddr_un structure and
     * add to this the length of the pathname, not including the terminating
     * null byte. Since implementations vary in what members precede
     * sun_path in the sockaddr_un structure, we use the offsetof() macro
     * from <stddef.h> to calculate the offset of the sun_path member from
     * the the start of the structure. If you look in <stddef.h>, you'll see
     * a definition similar to the following:
     * #define offsetof(TYPE, MEMBER) ((int)&((TYPE *)0)->MEMBER)
     */
    size = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    /* When we run this program, the bind() request succeeds, but if we run
     * the program a second time, we get an error, because the file already
     * exists. The program won't succeed again until we remove the file.
     *
     * 在我的linux机器上, stddef.h 头文件位于
     *      /usr/lib/gcc/x86_64-linux-gnu/4.7/include
     * 目录下.该文件中,offsetof()宏使用 __buildtin_offsetof() 宏来定义,但是
     * 查找__buildtin_offsetof()宏,没有找到它的定义,说是匹配二进制文件cc1,
     * cc1plus,可能代码没公开吧.
     */
    if (bind(sockfd, (struct sockaddr *)&un, size) < 0) {
        perror("bind failed");
        return 1;
    }

    /* 书中提到UNIX domain socket对应的socket文件不能被打开,下面调用
     * open()函数确实会报错: No such device or address, 即错误码ENXIO.
     */
    if (open(filename, O_RDWR) < 0) {
        perror("open foo.socket error");
    }

    printf("UNIX domain socket bound\n");
    if (unlink(filename) < 0) {
        perror("unlink foo.socket error");
        return 1;
    }

    return 0;
}
