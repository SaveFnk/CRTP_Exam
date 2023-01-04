#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <semaphore.h>

#include "BufferData.h"

#include "actor.c"
#include "producer.c"
#include "consumer.c"

struct BufferData *sharedBuf;
sem_t *semaphore;
int msgId;

int main(int argc, char *args[])
{
    sharedBuf = (struct BufferData *)malloc(sizeof(struct BufferData));

    int i, nConsumers;
    pid_t pids[MAX_PROCESSES];

    if (argc != 2)
    {
        printf("Usage: prodcons <nConsumers>\n");
        exit(0);
    }

    sscanf(args[1], "%d", &nConsumers);

    sem_init(&sharedBuf->mutexSem, 1, 1);
    sem_init(&sharedBuf->dataAvailableSem, 1, 0);
    sem_init(&sharedBuf->roomAvailableSem, 1, 1);
    sem_init(&sharedBuf->actorSem, 1, 0);

    /* Initialize message queue */
    msgId = msgget(IPC_PRIVATE, 0666);
    if (msgId == -1)
    { // error
        perror("msgget");
        exit(0);
    }
    /* Launch producer process */
    pids[0] = fork();
    if (pids[0] == 0)
    {
        /* Child process */
        producer(sharedBuf, msgId);
        exit(0);
    }
    /* Launch actor process */
    pids[1] = fork();
    if (pids[1] == 0)
    {
        /* Child process */
        actor(sharedBuf, msgId);
        exit(0);
    }
    /* Launch consumer processes */
    for (i = 0; i < nConsumers && i < MAX_PROCESSES - 1; i++)
    {
        pids[i + 2] = fork();
        if (pids[i + 2] == 0)
        {
            // ##### consumer();
            consumer(sharedBuf, i, msgId);
            exit(0);
        }
    }
    /* Wait process termination */
    for (i = 0; i <= nConsumers; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
    return 0;
}