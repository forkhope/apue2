#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>        /* getgrnam() */
#include <unistd.h>     /* ttyname(), chown() */
#include <sys/stat.h>   /* chmod() */

/* BSD风格的伪终端命令方式为: master: /dev/ptyMN, slave: /dev/ttyMN.其中,
 * M 和 N 的命名方式为: M: p-z, a-e, 共16个. N: 0-9, a-f, 共16个.
 * In out version of posix_openpt(), we have to determine the first
 * available PTY master device. To do this, we start at /dev/ptyp0 and
 * keep tyring until we successfully open a PTY master or until we run out
 * of devices. We can get two different errors from open(): EIO means that
 * the device is already in use; ENOENT means that the device doesn't exist.
 * In the latter case, we can terminate the search, as all pesudo terminals
 * are in use. Once we are able to open a PTY master, say /dev/ptyMN, the
 * name of the corresponding slave is /dev/ttyMN. On Linux, if the name of
 * the PTY master is /dev/pty/mXX, then the name of the corresponding PTY
 * slave is /dev/pty/sXX.
 */
int posix_openpt_l(int flags)
{
    int fd;
    char *ptr1, *ptr2;
    char ptm_name[16];

    strcpy(ptm_name, "/dev/ptyXY");
    /* array index: 0123456789 (for references in following code) */
    for (ptr1 = "pqrstuvwxyzPQRST"; *ptr1 != '\0'; ++ptr1) {
        ptm_name[8] = *ptr1;
        for (ptr2 = "0123456789abcdef"; *ptr2 != '\0'; ++ptr2) {
            ptm_name[9] = *ptr2;

            /* Try to open the master. */
            if ((fd = open(ptm_name, flags)) < 0) {
                if (errno == ENOENT)    /* different from EIO */
                    return -1;          /* out of pty devices */
                else
                    continue;           /* try next pty device */
            }
            return fd;      /* got it, return fd of master */
        }
    }
    errno = EAGAIN;
    return -1;              /* out of pty devices */
}

char *ptsname_l(int fd)
{
    static char pts_name[16];
    char *ptm_name;

    if ((ptm_name = ttyname(fd)) == NULL)
        return NULL;
    strncpy(pts_name, ptm_name, sizeof(pts_name));
    pts_name[sizeof(pts_name) - 1] = '\0';

    /* Linux下需要会执行下面的 if 分支 */
    if (strncmp(pts_name, "/dev/pty/", 9) == 0)
        pts_name[9] = 's';  /* change /dev/pty/mXX to /dev/pty/sXX */
    else
        pts_name[5] = 't';   /* change "pty" to "tty" */
    return pts_name;
}

int grantpt_l(int fd)
{
    struct group *grptr;
    int gid;
    char *pts_name;

    pts_name = ptsname_l(fd);
    if ((grptr = getgrnam("tty")) != NULL)
        gid = grptr->gr_gid;
    else
        gid = -1;       /* group tty is not in the group file */

    /* The following two call won't work unless we're the superuser. */
    if (chown(pts_name, getuid(), gid) < 0)
        return -1;
    return chmod(pts_name, S_IRUSR | S_IWUSR | S_IWGRP);
}

int unlockpt_l(int fd)
{
    return 0;   /* nothing to do */
}

int ptym_open(char *pts_name, int pts_namesz)
{
    char *ptr;
    int fd;

    /* Return the name of the master device so that on failure the caller
     * can print an error message. Null terminate to handle case where
     * string length > pts_namesz.
     */
    strncpy(pts_name, "/dev/ptyXX", pts_namesz);
    pts_name[pts_namesz - 1] = '\0';
    if ((fd = posix_openpt_l(O_RDWR)) < 0)
        return -1;
    if (grantpt_l(fd) < 0) {  /* grant access to slave */
        close(fd);
        return -2;
    }
    if (unlockpt_l(fd) < 0) {   /* clear slave's lock flag */
        close(fd);
        return -3;
    }
    if ((ptr = ptsname_l(fd)) == NULL) {  /* get slave's name */
        close(fd);
        return -4;
    }

    /* Return name of slave. Null terminate to handle case where
     * strlen(ptr) > pts_namesz.
     */
    strncpy(pts_name, ptr, pts_namesz);
    pts_name[pts_namesz - 1] = '\0';
    return fd;      /* return fd of master */
}

int ptys_open(char *pts_name)
{
    int fd;

    if ((fd = open(pts_name, O_RDWR)) < 0)
        return -1;
    return fd;
}
