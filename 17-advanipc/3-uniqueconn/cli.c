#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stddef.h>     /* offsetof() */
#include <sys/socket.h>
#include <sys/un.h>     /* struct sockaddr_un */
#include <unistd.h>
#include <sys/stat.h>   /* S_URWXU */

#define CLI_PATH    "/var/tmp/"     /* +5 for pid = 14 chars */
#define CLI_PERM    S_IRWXU         /* rwx for user only */

/* Create a client endpoint and connect to a server.
 * Returns fd if all OK, <0 on error.
 */
int cli_conn(const char *srvname)
{
    int fd, err, errval;
    struct sockaddr_un un;
    socklen_t addrlen;

    /* create a UNIX domain stream socket */
    if ((fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
        return -1;
    }

    /* fill socket address structure with our address */
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    sprintf(un.sun_path, "%s%05d", CLI_PATH, getpid());
    addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(un.sun_path);

    unlink(un.sun_path);    /* in case it already exists */
    if (bind(fd, (struct sockaddr *)&un, addrlen) < 0) {
        errval = -2;
        goto errout;
    }

    if (chmod(un.sun_path, CLI_PERM) < 0) {
        errval = -3;
        goto errout;
    }

    /* fill socket address structure with server's address */
    memset(&un, 0, sizeof(un));
    un.sun_family = AF_UNIX;
    strcpy(un.sun_path, srvname);
    addrlen = offsetof(struct sockaddr_un, sun_path) + strlen(srvname);
    if (connect(fd, (struct sockaddr *)&un, addrlen) < 0) {
        errval = -4;
        goto errout;
    }

    return fd;
errout:
    err = errno;
    close(fd);
    errno = err;
    return errval;
}

int main(int argc, char *argv[])
{
    int fd, n;
    char buf[BUFSIZ];

    if (argc != 2) {
        printf("usage: %s servicename\n", argv[0]);
        return 1;
    }

    printf("the pid of client is %d\n", (int)getpid());

    /* We then have to fill in another sockaddr_un structure, this time
     * with the well-known pathname of the server. Finallly, we call the
     * connect() function to initiate the connection with the server.
     */
    if ((fd = cli_conn(argv[1])) < 0) {
        perror("cli_conn: error");
        return 1;
    }

    if (send(fd, "tian\n", 5, 0) != 5) {
        perror("cli: send error");
        return 1;
    }
    printf("send success\n");

    /* 这里不要写为 while, 否则这么recv()函数将会一直阻塞 */
    if ((n = recv(fd, buf, BUFSIZ, 0)) > 0) {
        write(STDOUT_FILENO, buf, n);
    }
    if (n < 0) {
        perror("cli: recv error");
        return 1;
    }
    printf("recv success\n");

    return 0;
}
