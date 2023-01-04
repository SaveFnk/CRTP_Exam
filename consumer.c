#include "BufferData.h"

static void consumer(struct BufferData *sharedBuf, int ID, int msgId)
{
    int item;
    int retSize;
    struct msgbuf msgOut;

    while (1)
    {

        sem_wait(&sharedBuf->dataAvailableSem); // 1->0
        sem_wait(&sharedBuf->mutexSem);         // 1->0

        retSize = msgrcv(msgId, &msgOut, sizeof(int), 0, 0);

        if (retSize == -1)
        {
            perror("error msgrcv");
            exit(0);
        }
        item = msgOut.item;
        printf("consumer %d: item: %d\n", ID, item);

        /*item = sharedBuf->buffer[sharedBuf->readIdx];
        sharedBuf->buffer[sharedBuf->readIdx] = EMPTY; // consume
        sharedBuf->readIdx = (sharedBuf->readIdx + 1) % BUFFER_SIZE;
        sharedBuf->ID = ID;*/

        sem_post(&sharedBuf->roomAvailableSem); // 0->1
        sem_post(&sharedBuf->actorSem);         // 0->1
        // consumare
    }
}