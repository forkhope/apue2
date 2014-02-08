#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/* man msgsnd 手册以及书中都提到, msgsnd(), msgrcv() 函数发送的消息一般
 * 都定义为如下的结构体形式. The type field must have a strictly positive
 * integer value. This value can be used by the receiving process for
 * message selection. 书中对此的描述是: The message type can be used by the
 * receiver to fetch messages in an order other than first in, first out.
 */
struct message {
    long type;
    char msgdata[1024];
};

/* A message queue is a linked list of messages stored within the kernel
 * and identified by a message queue identifier. We'll call the message
 * queue just a queue and its identifier a queue ID.
 * A new queue is created or an existing queue opened by msgget(). New
 * messages are added to the end of a queue by msgsnd(). Every message has
 * a positive long integer type field, a non-negative length, and the
 * actual data bytes (corresponding to the length), all of which are
 * specified to msgsnd() when the message is added to a queue. Messages are
 * fetched from a queue by msgrcv(). We don't have to fetch the messages in
 * a first-in, first-out order. Instead, we can fetch messages based on
 * their type field.
 * 每个 message queue 有一个 msqid_ds 结构体和它相关联.
 */
int main(void)
{
    struct message msg;
    int msqid, i;
    key_t key;

    printf("This is the message sent side\n");

    /* XSI IPC (message queues, semaphores, and shared memory) 在内核中被
     * 关联到一个非负正数标识符.但是这个标识符属于IPC的内部标识,程序如何
     * 得到这个标识符呢? 类似于传递文件名给open()函数来得到文件描述符,程序
     * 可以传递一个key到 get函数 (msgget, semget, 和 shmget) 来得到标识符.
     * key 通常是 long integer 类型的值,当我们传递一个长整型的值给内核时,
     * 内核会将这个长整型的值转换为一个标识符.即类似于文件名,key可以是程序
     * 自己指定的. 方法有三种: (1) 使用系统定义的 IPC_PRIVATE; (2)自己任意
     * 指定一个长整型的值,如 0x20130503,但是这个值有可能已经在使用了; (3)
     * 调用ftok()函数来得到一个key的值,这个值也有可能会冲突.
     * #include <sys/ipc.h>
     * key_t ftok(const char *path, int id);
     *          Returns: key if OK, (key_t)-1 on error.
     * The path argument must refer to an existing file. Only the lower 8
     * bits of id are used when generating the key. The key created by
     * ftok() is usually formed by taking parts of the st_dev and st_ino
     * fields in the stat structure corresponding to the given pathname and
     * combining them with the project ID. 即, ftok()函数不会对path指向的
     * 文件造成影响,只是读取了该文件对应的stat结构体中一些成员的值.
     */
    if ((key = ftok(".", 'l')) < 0) {
        printf("can't ftok: %m\n");
        exit(1);
    }
    printf("the key is %#x\n", key);

    /* The first function normally called is msgget() to either open an
     * existing queue or create a new queue.
     * #include <sys/msg.h>
     *          Returns: message queue ID if OK, -1 on error.
     * 如果key对应的identifier还不存在时,需要指定IPC_CREAT标志,如果不指定,
     * 则会报错:No such file or directory,即错误码ENOENT,后面再或上0660是
     * 为了指定该消息队列的权限. msgget()函数返回的值类似于文件描述符,程序
     * 通过这个值来操作这个消息队列.
     *
     * On success, msgget() returns the non-negative queue ID. This value
     * is then used with the other three message queue function.
     */
    if ((msqid = msgget(key, IPC_CREAT | 0660)) < 0) {
        printf("can't msgget: %m\n");
        exit(1);
    }
    printf("the msqid is %d\n", msqid);

    /* 书中提到: If we create a message queue, place some messages on the
     * queue, and then terminate, the message queue and its contents are
     * not deleted. They remain in the system until specifically read or
     * deleted by some process calling msgrcv() or msgcrl(), by someone
     * executing the ipcrm command, or by the system being rebooted.
     * man ipcrm 描述了该命令是: remove a message queue, semaphore set or
     * shared memory id. 同时,可以使用ipcs命令来列出系统当前的IPC属性.
     * 这个程序执行后,执行 ipcs -q 查看消息队列属性,可以看到一个key为
     * 0x6c06081e (该程序创建的key,实际的值可能会发生变化) 的消息队列中有
     * 7 个消息,然后再执行自己编写的 msgrcv 程序读取消息后,消息数目变为0
     */
    for (i = 1; i < 8; ++i) {
        /* 注意,在调用msgsnd()时,消息类型要求是正数,即要大于0 (0不是正数),
         * 在开始写的时候,i 的初始是0,执行时一直报错:Invalid argument,即
         * 错误码EINVAL. 注意, 多个消息的 type 值可以相等,此时调用msgrcv()
         * 函数来获取值为这个type的消息时,会返回消息队列中值为type的第一
         * 条消息.
         */
        msg.type = i;
        sprintf(msg.msgdata, "put the %dth message", i);
        
        /* Data is placed onto a message queue by calling msgsnd().
         * #include <sys/msg.h>
         * int msgsnd(int msqid, const void *ptr, size_t nbytes, int flag)
         *      Returns: 0 if Ok, -1 on error.
         * Each message is composed of a positive long integer type field,
         * a non-negative length (nbytes), and the actual data bytes.
         * Messages are always placed at the end of the queue.
         * The ptr argument points to a long integer that contains the
         * positive integer message type, and it is immediately followed by
         * the message data. 所以消息都定义为上面的 struct message 结构体的
         * 形式. flag 参数可以指定消息的一些行为,如指定IPC_NOWAIT,则当消息
         * 队列已满之时,msgsnd()函数将会报错返回,错误码为EAGAIN,如果没有指定
         * IPC_NOWAIT,则当消息队列已满时,msgsnd()函数会阻塞.
         */
        if (msgsnd(msqid, &msg, sizeof(msg), 0) < 0) {
            printf("can't msgsnd: %m\n");
            exit(1);
        }
    }

    exit(0);
}
