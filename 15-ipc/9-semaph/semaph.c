#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/wait.h>

/* semctl() 函数用到的联合 union semun 类型,需要代码中自己定义,否则编译会
 * 报错,提示找不到这个数据类型,即使包含 <sys/ipc.h> 和 <sys/types.h> 也是
 * 一样. 下面这个定义参考自 man semctl 手册中给出的定义.
 */
union semun {
    int val;
    struct semid_ds *buf;
    unsigned short *array;
    struct seminfo *__buf;
};

/* 信号量的一个简单例子 */
int main(void)
{
    int semid, number;
    union semun sem_union;
    struct sembuf sops;
    pid_t pid;

    /* The first function to call is semget() to obtain a semaphore ID.
     * #include <sys/sem.h>
     * ing semget(key_t key, int nsems, int flag);
     *          Returns: semaphore ID if OK, -1 on error
     * The number of semaphores in the set is nsems. If a new set is being
     * created, we must specify nsems. If we are referencing an existing
     * set, we can specify nsems as 0.
     * 根据书中如下的描述:
     * A semaphore is not simply a single non-negative value. Instead, we
     * have to define a semaphore as a set of one or more semaphore values.
     * When we create a semaphore, we specify the number of values in the
     * set.
     * 可知, semget()函数返回的信号量ID对应一个信号集的多个元素,接下来的
     * 调用其他信号量函数,如semctl(), semop()等,不但需要传入信号量ID,也要传
     * 入信号集的元素索引(从0开始),指定要操作的信号集元素.
     */
    if ((semid = semget((key_t)0x20130506, 1, IPC_CREAT | 0666)) < 0) {
        printf("semget error: %m\n");
        exit(EXIT_FAILURE);
    }

    /* The semctl() function is the catchall for various semaphore
     * operations.
     * #include <sys/sem.h>
     * int semctl(int semid, int semnum, int cmd, ... // union semun arg);
     *          Returns: 太多了,可查看man手册.
     * cmd参数指定要执行的操作,例如 SETVAL 表示要设置信号量的初始值. The
     * value is specified by arg.val.
     * 书中提到: The creation of a semaphore (semget()) is independent of
     * its initialization (semctl()). 所以创建一个信号量后,必须调用semctl()
     * 来设置信号量的初值.
     */
    sem_union.val = 1;
    if (semctl(semid, 0, SETVAL, sem_union) < 0) {
        printf("semctl error: %m\n");
        exit(EXIT_FAILURE);
    }

    if ((pid = fork()) < 0) {
        printf("fork error: %m\n");
        exit(EXIT_FAILURE);
    }
    else if (pid == 0) {        /* child */
        sops.sem_num = 0;       /* 表示要操作信号集中的第0个元素 */
        sops.sem_op = -1;       /* 表示要将信号量的值减去 1 */
        /* Whenever we specify the SEM_UNDO flag for a semaphore operation
         * and we allocate resources (a sem_op value less than 0), the
         * kernel remembers how many resources we allocated from that
         * particular semaphore (the absolute value of sem_op). When the
         * process terminates, either voluntarily or involuntarily, the
         * kernel checks whether the process has any outstanding semaphore
         * adjustments and, if so, applies the adjustment to the
         * corresponding semephore.
         * 即,SEM_UNDO选项只是自动撤销之前对信号量的操作,但不会自动删除该
         * 信号量.信号量是系统资源,不再需要之后,最好将它删掉.
         */
        sops.sem_flg = SEM_UNDO;

        /* The function semop() atomically performs an array of operations
         * on a semaphore set.
         * #include <sys/sem.h>
         * int semop(int semid, struct sembuf semoparray[], size_t nops);
         *      Returns: 0 if OK, -1 on error.
         * The semoparray argument is a pointer to an array of semaphore
         * operations, represented by sembuf structures:
         * struct sembuf {
         *      unsigned short sem_num; // member # in set (0,1,...,nsems-1)
         *      short sem_op;  // operation (negative, 0, or positive)
         *      short sem_flg; // IPC_WAIT, SEM_UNDO
         * };
         * The nops argument specifies the number of operations (elements)
         * in the array.
         * 关于 sem_op 取值的含义,看书或者看man手册吧,内容有点多.
         */
        if (semop(semid, &sops, 1) < 0) {
            printf("child: semop negative error: %m\n");
            exit(EXIT_FAILURE);
        }

        number = 10;
        printf("In the child, number = %d\n", number);

        /* 下面的代码和上面很相似,其实可以写成函数的形式,但先这样吧. */
        sops.sem_num = 0;
        sops.sem_op = 1;
        sops.sem_flg = SEM_UNDO;
        if (semop(semid, &sops, 1) < 0) {
            printf("child: semop positive error: %m\n");
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    /* parent */
    sops.sem_num = 0;
    sops.sem_op = -1;
    sops.sem_flg = SEM_UNDO;
    if (semop(semid, &sops, 1) < 0) {
        printf("positive: semop negative error: %m\n");
        exit(EXIT_FAILURE);
    }

    number = 5;
    printf("In the parent, number = %d\n", number);

    sops.sem_num = 0;
    sops.sem_op = 1;
    sops.sem_flg = SEM_UNDO;
    if (semop(semid, &sops, 1) < 0) {
        printf("parent: semop positive error: %m\n");
        exit(EXIT_FAILURE);
    }

    if (waitpid(pid, NULL, 0) < 0) {
        printf("waitpid error: %m\n");
        exit(1);
    }

    /* 删掉该信号量. 对 IPC_RMID 命令字的描述如下:
     * Remove the semaphore set from the system. This removal is immediate.
     * Any other process still using the semaphore will get an error of
     * EIDRM on its next attempted operation on the semaphore.
     */
    if (semctl(semid, 0, IPC_RMID) < 0) {
        printf("parent: semctl IPC_RMID error: %m\n");
        exit(1);
    }

    exit(EXIT_SUCCESS);
}
