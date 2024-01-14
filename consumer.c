//#pragma once
#include "BufferData.h"

static void consumer(struct BufferData *sharedBuf, int ID, int bufId)
{
    int item;
    int retSize;
    struct msgbuf msgOut;

    while (1)
    {
        //consumer
        sem_wait(&sharedBuf->dataAvailableSem); // 1->0
        sem_wait(&sharedBuf->mutexSem);         // 1->0

        retSize = msgrcv(bufId, &msgOut, sizeof(int), 0, IPC_NOWAIT);

        if (retSize == -1)
        {
            perror("error msgrcv");
            //exit(0);
        }
        else{
            item = msgOut.item;
            printf("consumer %d: item: %d\n", ID, item);
        }

        sem_post(&sharedBuf->roomAvailableSem); // 0->1
        sem_post(&sharedBuf->actorSem);         // 0->1
        sleep(1);
    }
}