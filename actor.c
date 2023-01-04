
#include "BufferData.h"

int consumerCount[MAX_PROCESSES];
int producerCount = 0;

static void actor(struct BufferData *sharedBuf, int msgId)
{
    int item;
    while (1)
    {
        printf("actoraa");
        sem_wait(&sharedBuf->actorSem); // 1->0
        printf("actor");
        struct msqid_ds info;

        msgctl(msgId, IPC_STAT, &info);

        printf("last user: %l", info.msg_lrpid);

        // queue.len()

        /*int ID = sharedBuf->ID; // read the ID
        if (ID == -1)
        {
            // producer
            producerCount++;
        }
        else
        {
            // consumer
            consumerCount[ID]++;
        }*/

        sem_post(&sharedBuf->mutexSem); // 0->1
    }
}