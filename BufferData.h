#pragma once
#include <semaphore.h>
#include <sys/msg.h>

#define BUFFER_SIZE 50
#define EMPTY 0
#define MAX_PROCESSES 5

struct msgbuf
{
    long mtype;
    int item;
};
/*
struct msqid_ds
{
    long msg_lrpid; // process ID of last msgrcv()
};*/

struct BufferData
{
    // int readIdx;
    // int writeIdx;
    // int buffer[BUFFER_SIZE];
    // int ID;
    sem_t mutexSem;
    sem_t dataAvailableSem;
    sem_t roomAvailableSem;
    sem_t actorSem;
};