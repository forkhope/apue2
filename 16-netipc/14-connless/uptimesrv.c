#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <syslog.h>
#include <netdb.h>      /* getaddrinfo(), gai_strerror() */
#include <sys/socket.h>
#include <unistd.h>     /* chdir(), setsid() */
#include <sys/stat.h>   /* umask() */
#include <fcntl.h>
#include <sys/resource.h>
#include <stdlib.h>

#define BUFLEN 128
#define MAXADDRLEN 256

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

void daemonize(const char *name)
{
    int fd0, fd1, fd2, i;
    struct rlimit rl;
    struct sigaction sa;
    pid_t pid;

    umask(0);

    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        perror("getrlimit error");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0) {   /* parent */
        exit(0);
    }
    setsid();    /* child */

    sa.sa_handler = SIG_IGN;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGHUP, &sa, NULL) < 0) {
        perror("sigaction error");
        exit(1);
    }

    if ((pid = fork()) < 0) {
        perror("fork error");
        exit(1);
    }
    else if (pid > 0) { /* parent */
        exit(0);
    }

    /* child again */
    if (chdir("/") < 0) {
        perror("chdir error");
        exit(1);
    }

    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; ++i)
        close(i);

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

    openlog(name, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "unexpected file descriptors: %d, %d, %d",
                fd0, fd1, fd2);
        exit(1);
    }
}

int initserver(int type, const struct sockaddr *addr,
        socklen_t addrlen, int qlen)
{
    int sockfd, err = 0;

    if ((sockfd = socket(addr->sa_family, type, 0)) < 0) {
        perror("socket error");
        return -1;
    }

    /* 指定bind()函数,将sockfd和具体的地址和端口绑定起来,这样服务器使用
     * sockfd作为参数来调用recvfrom()时,recvfrom()函数才知道要从哪里读取
     * 数据,才不会和其他进程的recvfrom()函数冲突.
     */
    if (bind(sockfd, addr, addrlen) < 0) {
        err = errno;
        goto errend;
    }

    if (type == SOCK_STREAM || type == SOCK_SEQPACKET) {
        if (listen(sockfd, qlen) < 0) {
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
    char buf[BUFLEN];
    char addr[MAXADDRLEN];
    socklen_t addrlen;
    FILE *fp;

    for (;;) {
        addrlen = MAXADDRLEN;
        if (recvfrom(sockfd, buf, BUFLEN, 0,
                    (struct sockaddr *)addr, &addrlen) < 0) {
            syslog(LOG_ERR, "ruptimed: recvfrom error: %s",
                   strerror(errno));
            exit(1);
        }

        if ((fp = popen("/usr/bin/uptime", "r")) == NULL) {
            sprintf(buf, "popen error: %s\n", strerror(errno));
            sendto(sockfd, buf, strlen(buf), 0,
                    (struct sockaddr *)addr, addrlen);
        }
        else {
            while (fgets(buf, BUFLEN, fp) != NULL)
                sendto(sockfd, buf, strlen(buf), 0,
                        (struct sockaddr *)addr, addrlen);
        }
        pclose(fp);
        exit(0);
    }
}

/* The program is the datagram version of the uptime server.
 * The server blocks in recvfrom() for a request for service. When a
 * request arrives, we save the requester's address and use popen() to run
 * the uptime command. We send the output back to the client using the
 * sendto() function, with the destination address set to the requester's
 * address.
 */
int main(void)
{
    int sockfd, n, err;
    struct addrinfo *ailist, *aip;
    struct addrinfo hint;
    char *name;

#ifdef _SC_HOST_NAME_MAX
    n = sysconf(_SC_HOST_NAME_MAX);
    if (n < 0)
#endif
        n = HOST_NAME_MAX;
    if ((name = malloc(n)) == NULL) {
        perror("malloc error");
        exit(1);
    }
    if (gethostname(name, n) < 0) {
        perror("gethostname error");
        exit(1);
    }
    daemonize("ruptimed");

    hint.ai_flags = AI_CANONNAME;
    hint.ai_family = 0;
    hint.ai_socktype = SOCK_DGRAM;
    hint.ai_protocol = 0;
    hint.ai_addrlen = 0;
    hint.ai_addr = NULL;
    hint.ai_canonname = NULL;
    hint.ai_next = NULL;
    if ((err = getaddrinfo(name, "ruptime", &hint, &ailist)) != 0) {
        syslog(LOG_ERR, "ruptimed: getaddrinfo error: %s",
                gai_strerror(err));
        exit(1);
    }
    for (aip = ailist; aip != NULL; aip = aip->ai_next) {
        if ((sockfd = initserver(SOCK_DGRAM, aip->ai_addr,
                        aip->ai_addrlen, 0)) >= 0) {
            serve(sockfd);
            exit(0);
        }
    }
    exit(1);
}
