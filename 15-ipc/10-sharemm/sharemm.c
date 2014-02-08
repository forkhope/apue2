#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

#define ARRAY_SIZE      40000
#define MALLOC_SIZE     100000
#define SHM_SIZE        100000
#define SHM_MODE        0600    /* user read/write */

char array[ARRAY_SIZE]; /* uninitialized data = bss */

/* Shared memory allows two or more processes to share a given region of
 * memory.
 * The first function called is usually shmget(), to obtain a shared
 * memory identifier.
 * #include <sys/shm.h>
 * int shmget(key_t key, size_t size, int flag);
 *      Returns: shared memory ID if OK, -1 on error
 * The size parameter is the size of the shared memory segment in bytes.
 * Implementations will usually round up the size to a multiple of the
 * system's page size, but if an application specifies size as a value other
 * than an integral multiple of the system's page size, the remainder of
 * the last page will be unavailable for use. 也就是,虽然系统会将size扩展到
 * 系统的页大小,但是扩展出来的这部分并不能被使用. If a new segment is being
 * created, we must specify its size. If we are referencing an existing
 * segment, we can specify size as 0. When a new segment is created, the
 * contents of the segment are initialized with zeros.
 * The shmctl function is the catchall for various shared memory operations
 * #include <sys/shm.h>
 * int shmctl(int shmid, int cmd, struct shmid_ds *buf);
 *      Returns: 0 if OK, -1 on error
 *
 * Once a shared memory segment has been created, a process attaches it to
 * its address space by calling shmat().
 * #include <sys/shm.h>
 * void *shmat(int shmid, const void *addr, int flag);
 *      Returns: pointer to shared memory segment if OK, -1 on error
 * The address in the calling process at which the segment is attached
 * depends on the addr argument and whether the SHM_RND bit is specified
 * in flag. If addr is 0, the segment is attached at the first available
 * address selected by the kernel. This is the recommended technique.
 * If the SHM_RDONLY bit is specified in flag, the segment is attached
 * read-only. Otherwise, the segment is attached read-write.
 * 
 * When we're done with a shared memory segment, we call shmdt() to detach
 * it. Note that this does not remove the identifier and its associated
 * data structure from the system. The identifier remains in existence until
 * some process specifically removes it by calling shmctl() with a command
 * of IPC_RMID.
 * #include <sys/shm.h>
 * int shmdt(void *addr);
 *      Returns: 0 if OK, -1 on error
 */
int main(void)
{
    char *ptr, *shmptr;
    int shmid;

    printf("array[]: around %lx to %lx\n", (unsigned long)array,
            (unsigned long)&array[ARRAY_SIZE]);
    printf("stack around %lx\n", (unsigned long)&shmid);

    if ((ptr = malloc(MALLOC_SIZE)) == NULL) {
        printf("malloc error: %m\n");
        exit(1);
    }
    printf("malloced from %lx to %lx\n", (unsigned long)ptr,
            (unsigned long)ptr+MALLOC_SIZE);

    if ((shmid = shmget(IPC_PRIVATE, SHM_SIZE, SHM_MODE)) < 0) {
        printf("shmget error: %m\n");
        exit(1);
    }

    if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1) {
        printf("shmat error: %m\n");
        exit(1);
    }

    printf("shared memory attached from %lx to %lx\n", 
            (unsigned long)shmptr, (unsigned long)shmptr+SHM_SIZE);;

    if (shmctl(shmid, IPC_RMID, 0) < 0) {
        printf("shmctl error: %m\n");
        exit(1);
    }

    exit(0);
}
