//#pragma once
#ifndef BUFFERDATA_H
#define BUFFERDATA_H


#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <sys/mman.h>
#include <sys/socket.h>

#include <semaphore.h>
#include <pthread.h>

#include <netdb.h>
#include <netinet/in.h>

#define EMPTY 0
#define MAX_PROCESSES 10
#define BUFFER_SIZE 256


struct msgbuf
{
    long mtype;
    int item;//[MAX_PROCESSES];
};

/* Buffer of semaphores */
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


#endif