#include "../headers/BufferData.h"
#include "../headers/Utils.h"
#include <stdio.h>

void actor(struct BufferData *sharedBuf, int msgId)
{
    int item;
    while (1)
    {
        sem_wait(&sharedBuf->actorSem); // actor semaphore: 1 --> 0
        printf("actor");

        struct msqid_ds info;
        msgctl(msgId, IPC_STAT, &info);

        printf("Queue state:\n");

        printf("# messages in queue: %ld\n", info.msg_qnum);
        printf("Space: %ld kB |", (long)info.msg_cbytes/1000);

        int ratio = (info.msg_cbytes / info.msg_qbytes) * 10;
        for (int i = 0; i < 10; i++) { i < ratio ? printf("█") : printf("░"); }
        printf("| /%ld kB\n", (long)info.msg_qbytes/1000);

        printf("Last user PID: %d\n", info.msg_lrpid);


        sem_post(&sharedBuf->mutexSem); // mutex semaphore: 0 --> 1
    }
}