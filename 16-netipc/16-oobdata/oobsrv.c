#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>

int g_connfd;

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif /* HOST_NAME_MAX */

int initserver(int type, struct sockaddr *addr, socklen_t len, int qlen)
{
    int sockfd, err;

    if ((sockfd = socket(addr->sa_family, type, 0)) < 0) {
        perror("socket error");
        return -1;
    }

    if (bind(sockfd, addr, len) < 0) {
        err = errno;
        goto errout;
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET)
        if (listen(sockfd, qlen) < 0) {
            err = errno;
            goto errout;
        }

    return sockfd;
errout:
    errno = err;
    close(sockfd);
    return -1;
}

void sig_urg(int signum)
{
    char buf[1024];
    int n, optval, optlen;
    static int status = 0;

    printf("\noobsrv: enter sig_urg\n");
    if (status == 0) {
        /* 使用MSG_OOB表示读取带外数据 */
        if ((n = recv(g_connfd, buf, sizeof(buf)-1, MSG_OOB)) < 0) {
            perror("recv error");
            return;
        }
        buf[n] = '\0';
        printf("sig_urg: n: %d, buf: %s\n", n, buf);
        status = 1;

        optval = 1;
        optlen = sizeof(int);
        /* 如果不设置socket描述符的属性为 SO_OOBINLINE,也不用MSG_OOB参数来
         * 调用recv()函数,则对方发送过来的out-of-band数据就不会被收到.而
         * 实际测试发现,设置SO_OOBINLINE属性后,由于这个测试程序中,对方会发
         * 送两次out-of-band数据,一次是 "e", 而上面在第一次的时候用MSG_OOB
         * 读取到了这个"e",并打印出来,第二次是"E",此时并没有用MSG_OOB来读取
         * out-of-band数据,实际打印结果中,"e"会被打印两次,"E"只被打印一次.
         * 猜测使用MSG_OOB参数读取out-of-band数据后,该数据还停留在TCP普通的
         * 队列中,只不过不带MSG_OOB参数的recv()函数读取不到它而已.
         */
        if (setsockopt(g_connfd,SOL_SOCKET,SO_OOBINLINE,&optval,optlen)<0)
            perror("setsockopt error");
    }
}

void recv_data(int sockfd)
{
    char buf[2014];
    int n;

    for (;;) {
        if ((g_connfd = accept(sockfd, NULL, NULL)) < 0) {
            perror("accept error");
            return;
        }

        /* 书中Section 14.6.2小节提到如何捕获异步IO信号 SIGURG.
         * 1. Establish a signal handle for SIGURG, by calling either
         * signal() or sigaction().
         * 2. Set the process ID or process group ID ot receive the signal
         * for the descriptor, by calling fcntl() with a command of F_SETOWN
         * 下面就是要为socket描述符设置所有权.但注意,服务器端一般会有两个
         * socket,一个是监听socket,一个是accept()返回的和客户端交互的socket,
         * 经过实际测试发现,要对accept()返回的socket设置所有权,而不是对监听
         * socket设置所有权.这里的异步IO信号是发生在被读写的socket上,而不是
         * 监听socket上.关键在于区别accept()返回的socket和监听socket的区别.
         */
        if (fcntl(g_connfd, F_SETOWN, getpid()) < 0) {
            perror("fcntl error");
            return;
        }

        while ((n = recv(g_connfd, buf, 1024, 0)) > 0
                || errno == EINTR) {
            write(STDOUT_FILENO, buf, n);
        }
        if (n < 0) {
            perror("recv error");
        }
        close(g_connfd);
        return;
    }
}

int main(void)
{
    int sockfd, err, n;
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    struct sigaction sa;
    char *host;

#ifdef _SC_HOST_NAME_MAX
    n = sysconf(_SC_HOST_NAME_MAX);
    if (n < 0)
#endif /* _SC_HOST_NAME_MAX */
        n = HOST_NAME_MAX;
    if ((host = malloc(n)) == NULL) {
        perror("malloc error");
        return 1;
    }

    if (gethostname(host, n) < 0) {
        perror("gethostname error");
        return 1;
    }

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(host, "ruptime", &hint, &ailist)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }

    /* 书中Section 3.14小节对fcntl()函数的F_SETOWN参数描述如下:
     * Set the process ID or process group ID to receive the SIGIO and
     * SIGURG signals. A positive arg specifies a process ID. A negative
     * arg implies a process group ID equal to the absolute value of arg.
     * 即, F_SETOWN参数用来哪个进程会接收fd文件描述符上的SIGIO和SIGURG
     * 信号.这两个信号是异步信号,由内核生成,并触发,只有指定的进程才会接收到
     *
     * 当带外数据到来时,会触发异步IO信号SIGURG,这个时候,程序需要捕获这个
     * 信号,并在信号处理函数中读取带外数据.
     */
    sa.sa_handler = sig_urg;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGURG, &sa, NULL) < 0) {
        perror("sigaction error");
        return 1;
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
                        aip->ai_addrlen, 10)) >= 0) {
            /* 注意,要接收SIGURG信号,需要用fcntl()函数来设置socket的
             * 所有权,下面为sockfd设置所有权,结果程序执行的时候,发现
             * 一直都没有接收到SIGURG信号.后来发现,应该是要对accept()
             * 函数返回的socket文件描述符设置所有权,才会接收到SIGURG
             * 信号.因为我们是要在接收数据的时候,接收到SIGURG信号,那么
             * 就要设置接收数据的那个socket描述符的所有权,而不是设置
             * 下面这个用于监听的socket描述符的所有权.
             */
            // if (fcntl(sockfd, F_SETOWN, getpid()) < 0) {
            //     perror("fcntl error");
            //     return 1;
            // }
            recv_data(sockfd);
            printf("\nrecv_data: end ....\n");
            return 0;
        }
    }

    printf("initserver error: %s\n", strerror(errno));
    return 1;
}
