#include <stdio.h>
#include <string.h>
#include <stdlib.h> /* grantpt(), unlockpt(), posix_openpt(), etc. */
#include <fcntl.h>  /* O_RDWR, etc. */
#include <unistd.h>     /* close() */
#include <sys/ioctl.h>  /* ioctl */
#include <sys/stat.h>   /* chmod() */

/* The posix_openpt() function opens an unused pseudo-terminal master
 * device, returning a file descriptor that can be used to refer to that
 * device. The flags argument is a bit mask that ORs together zero or more
 * of the following flags:
 * O_RDWR:  Open the device for both reading and writing. It is usual to
 *          specify this flag.
 * O_NOCTTY:
 *          Do not make this device the controlling terminal for the
 *          process.
 *
 * On success, posix_openpt() returns a nonnegative file descriptor which
 * is the lowest numbered unused descriptor. On failure, -1 is returned,
 * and errno is set to indicate the error.
 *
 * Calling posix_openpt() creates a pathname for the corresponding pseudo-
 * terminal slave device. The pathname of the slave device can be obtained
 * using ptsname(). The slave device pathname exists only as long as the
 * master device is open.
 */
int posix_openpt_l(int flags)
{
    int fdm;

    fdm = open("/dev/ptmx", flags);
    return fdm;
}

/* The ptsname() function returns the name of the slave pseudo-terminal
 * device corresponding to the master referred to by fd.
 *
 * On success, ptsname() returns a pointer to a string in static storeage
 * which will be overwritten by subsequent calls. This pointer must not
 * be freed. On failure, a NULL pointer is returned.
 */
char *ptsname_l(int fd)
{
    int sminor;
    static char pts_name[16];

    /* 查看 /usr/include/asm-generic/ioctls.h, 里面对 TIOCGPTN 的注释为:
     *  Get Pty Number (of pty-mux device).
     */
    if (ioctl(fd, TIOCGPTN, &sminor) < 0)
        return NULL;
    snprintf(pts_name, sizeof(pts_name), "/dev/pts/%d", sminor);
    return pts_name;
}

/* The grantpt() function changes the mode and owner of the slave pseudo-
 * terminal device corresponding to the master pseudo-terminal referred to
 * by fd. The user ID of the slave is set to the real UID of the calling
 * process. The group ID is set to an unspecified value (e.g., tty). The
 * mode of the slave is set to 0620 (crx--w-----).
 *
 * The behavior of grantpt() is unspecified if a signal handler is
 * installed to catch SIGCHID signals.
 *
 * When successful, grantpt() returns 0, Otherwise, it returns -1 and sets
 * errno appropriately.
 */
int grantpt_l(int fd)
{
    char *pts_name;

    pts_name = ptsname_l(fd);
    /* 上面提到, grantpt()函数会改变slave device的权限和拥有者,但是该实现
     * 中只改变了slave device的权限.书中提到了这个原因: On Linux, the PTY
     * slave device is already owned by group tty, so all we need to do in
     * grantpt() is ensure that the permissions are correct.
     */
    return chmod(pts_name, S_IRUSR | S_IWUSR | S_IWGRP);
}

/* The unlockpt() function unlocks the slave pseudo-terminal device
 * corresponding to the master pseudo-terminal referred to by fd.
 *
 * unlockpt() should be called before opening the slave side of a
 * pseudo-terminal. 在打开slave device之前,需要先调用unlockpt()函数.
 *
 * When successful, unlockpt() returns 0. Otherwise, it returns -1 and
 * sets errno appropriately.
 */
int unlockpt_l(int fd)
{
    int lock = 0;

    /* 查看 /usr/include/asm-generic/ioctls.h, 里面对 TIOCSPTLCK 的注释为:
     *  Lock/unlock Pty.
     */
    return (ioctl(fd, TIOCSPTLCK, &lock));
}

/* Linux supports the BSD method for accessing pseudo terminals.
 * However, Linux also supports a clone-style interface to pseudo terminals
 * using /dev/ptmx (but this is not a STREAMS device). The clone interface
 * requires extra steps to identify and unlock a slave device.
 *
 * The ptym_open() function determines the next available PTY master and
 * opens the device. The caller must allocate an array to hold the name of
 * either the master or the slave; if the call succeeds, the name of the
 * corresponding slave is returned through pts_name. This name is then
 * passed to ptys_open(), which opens the slave device. The length of the
 * buffer in bytes is passed in pts_namesz so that the ptym_open() function
 * doesn't copy a string that is longer than the buffer.
 */
int ptym_open(char *pts_name, int pts_namesz)
{
    char *ptr;
    int fd;

    /* Return the name of the master device so that on failure
     * the caller can print an error message. Null terminate to
     * handle case where string length > pts_namesz.
     */
    strncpy(pts_name, "/dev/ptmx", pts_namesz);
    pts_name[pts_namesz - 1] = '\0';

    fd = posix_openpt_l(O_RDWR);
    if (fd < 0)
        return -1;
    if (grantpt_l(fd) < 0) {    /* grant access to slave */
        close(fd);
        return -2;
    }
    if (unlockpt_l(fd) < 0) {   /* clear slave's lock flag */
        close(fd);
        return -3;
    }
    if ((ptr = ptsname_l(fd)) == NULL) {    /* get slave's name */
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

/* Returns: file descriptor of PTY slave if OK, -1 on error. */
int ptys_open(char *pts_name)
{
    int fd;

    if ((fd = open(pts_name, O_RDWR)) < 0)
        return -1;
    return fd;
}
