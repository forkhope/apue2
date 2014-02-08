#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h> /* struct msghdr */

#define CONTROLLEN CMSG_LEN(sizeof(int))

static struct cmsghdr *cmptr = NULL;

int recv_fd(int fd)
{
    int newfd, nr;
    char *ptr;
    char buf[BUFSIZ];
    struct iovec iov[1];
    struct msghdr msg;

    iov[0].iov_base = buf;
    iov[0].iov_len = sizeof(buf);
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL) {
        perror("malloc error");
        return -1;
    }
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;
    if ((nr = recvmsg(fd, &msg, 0)) < 0) {
        perror("recvmsg error");
        return -1;
    }
    else if (nr == 0) {
        printf("connection closed by server\n");
        return -1;
    }
    printf("the return value of recvmsg is: %d\n", nr);

    for (ptr = buf; ptr < &buf[nr]; ++ptr) {
        printf("buf[%ld]: %d\n", ptr-buf, *ptr);
    }

    printf("CONTROLLEN: %lu\n", CONTROLLEN);
    printf("msg.msg_controllen: %lu\n", msg.msg_controllen);
    if (msg.msg_controllen != CONTROLLEN) {
        printf("the msg.msg_controllen != CONTROLLEN\n");
    }

    /* 实际测试结果,打印出来的 newfd 的值总是 0,这是有问题的,但目前不知道
     * 原因在哪里.传递的文件描述符好像没有被接收到.
     */
    newfd = *(int *)CMSG_DATA(cmptr);
    printf("the newfd is: %d\n", newfd);

    return newfd;
}

int connect_retry(int sockfd, const struct sockaddr *addr, socklen_t len)
{
    const int MAXTIMES = 8;
    int nsecs;

    for (nsecs = 1; nsecs <= MAXTIMES; nsecs <<= 1) {
        if (connect(sockfd, addr, len) == 0)
            return 0;

        if (nsecs <= MAXTIMES / 2)
            sleep(nsecs);
    }

    return -1;
}

int main(int argc, char *argv[])
{
    int sockfd, err, newfd;
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;

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
        printf("cli: getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }

    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = socket(aip->ai_family, SOCK_STREAM, 0)) < 0)
            err = errno;
        if (connect_retry(sockfd, aip->ai_addr, aip->ai_addrlen) < 0)
            err = errno;
        else {
            newfd = recv_fd(sockfd);
            printf("newfd: %d\n", newfd);
            return 0;
        }
    }

    printf("can't contact %s: %s\n", argv[1], strerror(errno));
    return 1;
}
