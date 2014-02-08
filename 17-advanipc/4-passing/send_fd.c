#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

#define CONTROLLEN CMSG_LEN(sizeof(int))
#define PERM_MODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif /* HOST_NAME_MAX */

static struct cmsghdr *cmptr = NULL;

/* 这个函数有两个参数,第一个参数 fd 是socket描述符,本端通过该fd和对端交互
 * 数据. 第二个参数 fd_to_send 是要传递给对端的文件描述符.
 */
int send_fd(int fd, int fd_to_send)
{
    struct iovec iov[1];
    struct msghdr msg;
    char buf[2];
    
    iov[0].iov_base = buf;
    iov[0].iov_len = 2;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;
    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    if (cmptr == NULL && (cmptr = malloc(CONTROLLEN)) == NULL) {
        perror("malloc error");
        return -1;
    }
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    cmptr->cmsg_len = CONTROLLEN;
    msg.msg_control = cmptr;
    msg.msg_controllen = CONTROLLEN;
    *(int *)CMSG_DATA(cmptr) = fd_to_send;
    buf[0] = fd;
    buf[1] = fd_to_send;

    printf("CONTROLLEN: %lu\n", CONTROLLEN);
    printf("aaaa: %d\n", *(int *)CMSG_DATA(cmptr));

    if (sendmsg(fd, &msg, 0) != 2)
        return -1;
    return 0;
}

int initserver(int type, const struct sockaddr *addr,
        socklen_t addrlen, int qlen)
{
    int sockfd, err = 0;

    if ((sockfd = socket(addr->sa_family, type, 0)) < 0) {
        perror("socket error");
        return -1;
    }

    if (bind(sockfd, addr, addrlen) < 0) {
        err = errno;
        goto errend;
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(sockfd,qlen) < 0) {
            err = errno;
            goto errend;
        }
    }
    return sockfd;

errend:
    close(sockfd);
    errno = err;
    return -1;
}

void serve(int sockfd)
{
    int connfd, fd;

    if ((connfd = accept(sockfd, NULL, NULL)) < 0) {
        perror("accept error");
        exit(1);
    }
    printf("srv: connfd: %d\n", connfd);

    if ((fd = open("tempfile", O_RDWR|O_CREAT, PERM_MODE)) < 0) {
        perror("open error");
        exit(1);
    }
    printf("the fd of tempfile is: %d\n", fd);
    if (send_fd(connfd, fd) < 0) {
        perror("send_fd error");
    }
}

/* We must be more specific about what we mean by "passing an open file
 * descriptor" from one process to another. Recall Figure 3.7, which showed
 * two processes that have opened the same file. Although they share the
 * same v-node, each process has its own file table entry.
 * When we pass an open file descriptor from one process to another, we
 * want the passing process and the receiving process to share the same
 * file table entry.
 * Technically, we are passing a pointer to an open file table entry from
 * one process to another. This pointer is assigned the first available
 * descriptor in the receiving process. (Saying that we are passing an open
 * descriptor mistakenly gives the impression that the descriptor number in
 * the receiving process is the same as in the sending process, which
 * usually isn't true.) Having two processes share an open file table is
 * exactly what happens after a fork().
 *
 * 接收进程调用recvmsg()在UNIX域套接字上接收文件描述符,通常接收进程接收到
 * 的描述符的值和发送进程中的描述符的值不通,这是正常的.传递描述符不是传递
 * 描述符的值,而是在接收进程中建立一个新的描述符,指向内核文件表中与发送进
 * 程发送的描述符相同的项.
 *
 * What normally happeds when a descriptor is passed from one process to
 * another is that the sending process, after possing the descriptor, then
 * closes the descriptor. Closing the descriptor by the sender doesn't
 * really close the file or device, since the descriptor is still considered
 * open by the receiving process (even if the receiver hasn't specifically
 * received the descriptor yet).
 */
int main(void)
{
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    int sockfd, err, n;
    char *name;

#ifdef _SC_HOST_NAME_MAX
    n = sysconf(_SC_HOST_NAME_MAX);
    if (n < 0)
#endif
        n = HOST_NAME_MAX;
    if ((name = malloc(n)) == NULL) {
        perror("malloc error");
        return 1;
    }
    if (gethostname(name, n) < 0) {
        perror("gethostname error");
        return 1;
    }
    printf("name: %s\n", name);

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_STREAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(name, "ruptime", &hint, &ailist)) != 0) {
        printf("getaddrinfo error: %s\n", gai_strerror(err));
        return 1;
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_STREAM, aip->ai_addr,
                        aip->ai_addrlen, QLEN)) >= 0) {
            serve(sockfd);
            return 0;
        }
    }

    return 0;
}
