#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t len)
{
    int n;
    const int MAX_SECONDS = 8;

    for (n = 1; n <= MAX_SECONDS; n <<= 1) {
        if (connect(sockfd, addr, len) == 0)
            return 0;
        if (n <= MAX_SECONDS / 2)
            sleep(n);
    }
    return -1;
}

void send_data(int sockfd)
{
    if (send(sockfd, "abc", 3, 0) < 0) {
        perror("send error");
    }

    /* 经实际测试,下面的 sleep(1) 一定要有,否则执行结果将不会
     * 接收到带外数据.目前原因不明,可能是不加这个sleep(1)时,程序
     * 结束得太快,导致SIGURG信号还没有生成,服务器那边就结束了.
     */
    sleep(1);
    /* 使用MSG_OOB标志,表示要发送带外数据 */
    if (send(sockfd, "e", 1, MSG_OOB) < 0) {
        perror("send MSG_OOB error");
    }

    if (send(sockfd, "123", 3, 0) < 0) {
        perror("send error");
    }

    sleep(1);
    if (send(sockfd, "E", 1, MSG_OOB) < 0) {
        perror("send MSG_OOB  'E' error");
    }

    printf("send_data end.......\n");
}

/* Out-of-band data is an optional feature supported by some communication
 * protocols, allowing higher-priority delivery of data than normal. Out-
 * of-band data is sent ahead of any data that is already queued for
 * transmission. TCP supports out-of-band data, but UDP doesn't.
 * TCP refers to out-of-band data as "urgent" data. TCP supports only a
 * single byte of urgent data, but allows urgent data to be delivered out of
 * band from the normal data delivery mechanisms. To generate urgent data,
 * we specify the MSG_OOB flag to any of the three send() functions. If we
 * send more than one byte with the MSG_OOB flag, the last byte will be
 * treated as the urgent-data byte.
 *
 * When urgent data is received, we are sent the SIGURG signal if we have
 * arranged for signal generation by the socket. In Section 3.14 and 14.6.2,
 * we saw that we could use the F_SETOWN command to fcntl() to set the
 * ownership of a socket. If the third argument to fcntl is posotive, it
 * specifies a process ID. If it is a negative value other than -1, it
 * represents the process group ID. 从书中描述来看,SIGURG是异步信号,应该是
 * 需要先执行一个类似于注册的操作之后,系统才知道要把这个信号发给哪个进程,
 * 使用F_SETOWN参数来调用fcntl()函数,就是执行这个类似注册的操作.
 *
 * TCP supports the notion of an urgent mask: the point in the normal data
 * stream where the urgent data would go. We can choose to receive the
 * urgent data inline with the normal data if we use the SO_OOBINLINE socket
 * option. To help us identify when we have reached the urgent mark, we can
 * use the sockatmark() function.
 * #include <sys/socket.h>
 * int sockatmask(int sockfd);
 *      Returns: 1 if at mask, 0 if not at mask, -1 on error
 * When the next byte to be read is where the urgent mask is located,
 * sockatmask() will return 1.
 *
 * When out-of-band data is present in a socket's read queue, the select()
 * function will return the file descriptor as having an exception condition
 * pending. We can choose to receive the urgent data inline with the normal
 * data, or we can use the MSG_OOB flag with one of the recv() functions to
 * receive the urgent data ahead of any other queue data. TCP queues only
 * one byte of urgent data. If another urgent byte arrives before we receive
 * the current one, the existing one is discarded.
 */
int main(int argc, char *argv[])
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err;

    if (argc != 2) {
        printf("usage: %s hostname\n", argv[0]);
        return 1;
    }

    hint.ai_flags = 0;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0)
            err = errno;
        if (connect_retry(sockfd, aip->ai_addr, aip->ai_addrlen) < 0)
            err = errno;
        else {
            send_data(sockfd);
            return 0;
        }
    }

    printf("socket or connect error: %s\n", strerror(err));
    return 1;
}
