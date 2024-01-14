/* CRTP lab server */

#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* Receive routine: use recv to receive from socket and manage
   the fact that recv may return after having read less bytes than
   the passed buffer size
   In most cases recv will read ALL requested bytes, and the loop body
   will be executed once. This is not however guaranteed and must
   be handled by the user program. The routine returns 0 upon
   successful completion, -1 otherwise */
static int receive(int sd, char *retBuf, int size) {
  int totSize, currSize;
  totSize = 0;
  while (totSize < size) {
    currSize = recv(sd, &retBuf[totSize], size - totSize, 0);
    if (currSize <= 0)
      /* An error occurred */
      return -1;
    totSize += currSize;
  }
  return 0;
}

/* Handle an established  connection
   routine receive is listed in the previous example */
static void handleConnection(int currSd) {
  unsigned int netLen;
  int len;
  int exit_status = 0;
  char *command, *answer;
  for (;;) {
    /* Get the command string length
       If receive fails, the client most likely exited */
    if (receive(currSd, (char *)&netLen, sizeof(netLen)))
      break;
    /* Convert from network byte order */
    len = ntohl(netLen);
    command = malloc(len + 1);
    /* Get the command and write terminator */
    receive(currSd, command, len);
    //printf("Comando: %s\n", command);
    command[len] = 0;
    if (len > 0) {
      printf("%s", command);
      bzero(command, len);
    }
    /* Execute the command and get the answer character string */
  }
  /* The loop is most likely exited when the connection is terminated */
  printf("Connection terminated\n");
  close(currSd);
}

/* Main Program */
int main(int argc, char *argv[]) {
  int sd, currSd;
  int sAddrLen;
  int port;
  int len;
  unsigned int netLen;
  char *command, *answer;
  struct sockaddr_in sin, retSin;
  /* The port number is passed as command argument */
  if (argc < 2) {
    printf("Usage: server <port>\n");
    exit(0);
  }
  sscanf(argv[1], "%d", &port);
  /* Create a new socket */
  if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  /* set socket options REUSE ADDRESS */
  int reuse = 1;
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (const char *)&reuse,
                 sizeof(reuse)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");
#ifdef SO_REUSEPORT
  if (setsockopt(sd, SOL_SOCKET, SO_REUSEPORT, (const char *)&reuse,
                 sizeof(reuse)) < 0)
    perror("setsockopt(SO_REUSEPORT) failed");
#endif
  /* Initialize the address (struct sokaddr_in) fields */
  memset(&sin, 0, sizeof(sin));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr("127.0.0.1");
  sin.sin_port = htons(port);

  /* Bind the socket to the specified port number */
  if (bind(sd, (struct sockaddr *)&sin, sizeof(sin)) == -1) {
    perror("bind");
    exit(1);
  }
  /* Set the maximum queue length for clients requesting connection to 5 */
  if (listen(sd, 5) == -1) {
    perror("listen");
    exit(1);
  }
  sAddrLen = sizeof(retSin);
  /* Accept and serve all incoming connections in a loop */
  for (;;) {
    if ((currSd = accept(sd, (struct sockaddr *)&retSin, &sAddrLen)) == -1) {
      perror("accept");
      exit(1);
    }
    /* When execution reaches this point a client established the connection.
       The returned socket (currSd) is used to communicate with the client */
    printf("Connection received from %s\n", inet_ntoa(retSin.sin_addr));
    handleConnection(currSd);
  }

  return 0; // nrever reached
}
