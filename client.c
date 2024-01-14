/* CRTP lab client */

//#pragma once

#include "BufferData.h"

#define FALSE 0
#define TRUE 1

int consumerCount[MAX_PROCESSES];
int producerCount = 0;

static void actor(struct BufferData *sharedBuf, int bufId, int nConsumers, char *act) {

  // CLIENT
  char hostname[9] = "127.0.0.1";
  //int stopped = FALSE;
  int port = 8080;
  int sd;
  int len;
  unsigned int netLen;

  struct sockaddr_in sin;
  struct hostent *hp;

  /* Resolve the passed name and store the resulting long representation
    in the struct hostent variable */
  if ((hp = gethostbyname(hostname)) == 0) {
    perror("gethostbyname");
    exit(1);
  }
  /* fill in the socket structure with host information */
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = ((struct in_addr *)(hp->h_addr_list[0]))->s_addr;
  sin.sin_port = htons(port);
  /* create a new socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  /* connect the socket to the port and host
    specified in struct sockaddr_in */
  if (connect(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("connect");
    exit(1);
  }
  
  /* ACTOR -> every time someone do anything at msgbuf, sends to server what happened */
  int item, ID;
  struct msqid_ds info;
  bool fullArr = false;

  int users[nConsumers + 1];
  //users id in array (to check number and id are always same couple)
  for (int k = 0; k < nConsumers + 1; k++)
    users[k] = -1;

  bool tmp = true;

  while (1) {
    sem_wait(&sharedBuf->actorSem); // 1->0

    msgctl(bufId, IPC_STAT, &info);

    // Add id in "memory"
    for (int k = 1; k < nConsumers + 1; k++) {
      // if nobody has ever read the buffer
      if (info.msg_lrpid == 0)
        users[0] = info.msg_lspid;
      // if last user was producer
      if (info.msg_stime > info.msg_rtime) {
        ID = 0;
        break;
      }
      // other cases
      if (!fullArr) {
        // if more than one user but less than all users has read something
        if (users[k] == -1) {
          // printf("Assegnato il pid %d all'id %d\n", info.msg_lrpid, k);
          users[k] = info.msg_lrpid;
          ID = k;
          // if im adding the last consumer
          if (k == nConsumers) {
            fullArr = true;
            // printf("Ok visti tutti\n");
          }
          break;
        }
        // if i get same ID before array is full
        else if (users[k] == info.msg_lrpid) {
          ID = k;
          break;
        }
      }
      // search for existing ID when array is full
      else if (users[k] == info.msg_lrpid) {
        ID = k;
        break;
      }
    }

    // Output
    if (ID == 0) {
      // printf("Last user was producer\n");
      strcpy(act, "Last user was producer\n");
    } else {
      // printf("Last user was a consumer n. %d with pid %d\n", ID,
      // info.msg_lrpid);
      strcpy(act, "Last user was consumer\n");
    }
    sem_post(&sharedBuf->mutexSem); // 0->1

    /* Send first the number of characters in the command and then
   the command itself */
    len = strlen(act);
    /* Convert the integer number into network byte order */
    netLen = htonl(len);
    /* Send number of characters */
    if (send(sd, &netLen, sizeof(netLen), 0) == -1) {
      perror("send");
      exit(1);
    }
    /* Send the command */
    if (send(sd, act, len, 0) == -1) {
      perror("send");
      exit(0);
    }
    sleep(1);
  }
  /* Close the socket */
  close(sd);
  printf("Per qualche ragione l'attore è uscito\n");
}