#include <sys/msg.h>
#include <stdlib.h>
#include <stdio.h>

struct message {
    long type;
    char msgdata[1024];
};

int main(void)
{
    struct message msg;
    int msqid, i;
    key_t key;

    printf("This is the message received side\n");
    if ((key = ftok(".", 'l')) < 0) {
        printf("can't ftok: %m\n");
        exit(1);
    }
    printf("the key is %#x\n", key);

    if ((msqid = msgget(key, IPC_CREAT | 0660)) < 0) {
        printf("can't msgget: %m\n");
        exit(1);
    }
    printf("the msqid is %d\n", msqid);

    /* Messages are retrieved from a queue by msgrcv().
     * #include <sys/msg.h>
     * ssize_t msgrcv(int msqid, void *ptr, size_t nbytes, long type,
     *          int flag);
     *          Returns: size of data portion of message if OK, -1 on error
     * As with msgsnd(), the ptr argument points to a long integer (where
     * the message type of the returned message is stored) followed by a
     * data buffer for the actual message data. nbytes specifies the size
     * of the data buffer. The type argument lets us specify which message
     * we want.
     * type == 0: The first message on the queue is returned.
     * type > 0:  The first message on the queue whose message type equals
     * type is returned.
     * type < 0:  The first message on the queue whose message type is the
     * lowest value less than or equal to the absolute value of type is
     * returned.
     */
    for (i = 7; i > 0; --i) {
        if (msgrcv(msqid, &msg, sizeof(msg), i, 0) < 0) {
            printf("can't msgrcv: %m\n");
            exit(1);
        }
        printf("msg.type: %ld\t", msg.type);
        printf("msg.msgdata: %s\n", msg.msgdata);
    }

    exit(0);
}
