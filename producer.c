#include "BufferData.h"
//  roomAvailableSem=1
//  dataAvailableSem=0
//  mutexSem=1

static void producer(struct BufferData *sharedBuf, int msgId)
{
    int item = 1;
    struct msgbuf msg;
    msg.mtype = 0;
    while (1)
    {
        // producere

        sem_wait(&sharedBuf->roomAvailableSem); // 1->0
        sem_wait(&sharedBuf->mutexSem);         // 1->0
        printf("producer: item: %d\n", msgId);

        msg.item = item;
        int res = msgsnd(msgId, &msg, sizeof(int) + 1, 0);

        /*sharedBuf->ID = -1;
        for(int i=0; i < 5; i++){
            sharedBuf->buffer[sharedBuf->writeIdx] = item;
            sharedBuf->writeIdx = (sharedBuf->writeIdx + 1) % BUFFER_SIZE;
            sem_post(&sharedBuf->dataAvailableSem); // 0->N

        }*/

        printf("post %d", res);
        sem_post(&sharedBuf->actorSem); // 0->1

        // sem_post(&sharedBuf->mutexSem);
        // consumare
        item++;
    }
}