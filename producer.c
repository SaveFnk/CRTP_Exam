//#pragma once
#include "BufferData.h"
//  roomAvailableSem=1
//  dataAvailableSem=0
//  mutexSem=1

static void producer(struct BufferData *sharedBuf, int bufId) {
  int item = 0;
  struct msgbuf msg;
  msg.mtype = 1;
  
  while (1) {
    // producer
    sem_wait(&(sharedBuf->roomAvailableSem)); // 1->0
    sem_wait(&(sharedBuf->mutexSem));         // 1->0

    printf("producer: item: %d\n", item);

    //every time inserting enough items for every consumer to use on average one of them
    for (int i = 0; i < MAX_PROCESSES; i++) {
      msg.item = item;
      int res = msgsnd(bufId, &msg, sizeof(int), 0);
      if (res == -1) {
        perror("error msgsnd");
      }
      item++;
      sem_post(&sharedBuf->dataAvailableSem); // 0->N
    }

    sem_post(&(sharedBuf->actorSem)); // 0->1
    sleep(1);
  }
}