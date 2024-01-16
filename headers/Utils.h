#ifndef UTILS_H
#define UTILS_H

#include "BufferData.h"

// actor.c
void actor(struct BufferData *sharedBuf, int msgId);

// consumer.c
void consumer(struct BufferData *sharedBuf, int ID, int bufId);

// client.c
void client(struct BufferData *sharedBuf, int bufId, int nConsumers, char *act);

// producer.c
void producer(struct BufferData *sharedBuf, int bufId);

#endif