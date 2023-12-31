#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#define DEFAULT_PORT 2000
#define MAX_CLIENT_COUNT 50
#define MODBUSADRESS 3

void* serverWork(void* arg);

typedef struct 
{
  int socket;
  int number;
}client;


int main() {
  int soc;
  int lastClient = -1;

  soc = socket(AF_INET, SOCK_STREAM, 0);
  if(soc < 0){
   printf("Error: create socket\n");
   return 0;
  }
  struct sockaddr_in serverAddr;
  serverAddr.sin_port = htons(DEFAULT_PORT);
  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = htons(INADDR_ANY);
  int res = bind(soc, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if(res < 0){
   printf("Port is unavailable\n");
   return 0;
  }
  printf("Server is waiting for request\n");
  listen(soc, SOMAXCONN);
  pthread_t threads[MAX_CLIENT_COUNT];

  socklen_t serverSize = sizeof(serverAddr);
  while(1){
    lastClient++;
    int clientSocet = accept(soc, (struct sockaddr *)&serverAddr, &serverSize);
    if(clientSocet < 0){
      printf("Error of connection\n");
    }
    if(lastClient == MAX_CLIENT_COUNT) break;
    client c;
    c.socket = clientSocet;
    c.number = lastClient;
    pthread_create(&threads[lastClient], NULL, &serverWork, (void*)&c);
    sched_yield();
    
  }
  
  close(soc);
  pthread_exit(NULL); 
}


void* serverWork(void* arg) {
  client* clnt = (client *)arg;
  int soc = clnt->socket;
  int number = clnt->number;
  number++;

  unsigned char sendbuf[256];
  unsigned char recvnbuf[256];
  char* message;
  int i, j, lenbuf;
  unsigned short first, nums;
  unsigned short registers [10];
  for (int i = 0; i < 10; i++) {registers[i] = i + 1;}

  printf("\nConnection with client %d\n\n", number);
  while(1){
    i =recv(soc, recvnbuf, 256, 0);  

    if(i == -1){
      printf("receive error");
    }
    else if (i == 0){
      printf("Connection with client %d break\n", number);
      break;
    }
    else if(strcmp(recvnbuf, "check") == 0){
      printf("check");
      send(soc, "check", 5, 0);
      continue;
    }
    else if(strcmp(recvnbuf, "exit") == 0){
      printf("Connection with client %d closed\n", number);
      break;
    }
    else if(recvnbuf[6] != MODBUSADRESS){
      message = "Incorrect MODBUS adress";
      send(soc, message, strlen(message), 0);
    }
    else if(recvnbuf[7] & 0x80){
      message = "Incorrect MODBUS function code";
      send(soc, message, strlen(message), 0);
    }
    else if(first > 10){
      message = "Incorrect number of first register";
      send(soc, message, strlen(message), 0);
    }
    else if((10 - first - nums) < -1){
      message = "Incorrect nums of registers";
      send(soc, message, strlen(message), 0);
    }
    else {
      first = (recvnbuf[8]<<8) + recvnbuf[9];
      nums = (recvnbuf[10]<<8) + recvnbuf[11];

      for (i=0;i<5;i++) sendbuf[i] = 0;
      sendbuf[5] = 3+2*nums;
      sendbuf[6] = MODBUSADRESS;
      sendbuf[7] = 3;
      sendbuf[8] = 2*nums;

      j = 9;
      for (int k=first; k < first + nums + 1; k++)
      {
        sendbuf[j] = registers[k-1] >> 8;;
        sendbuf[j+1] = registers[k-1] & 0xff;
        j += 2;
      }

      lenbuf = 9+2*nums;

      i = send(soc, sendbuf, lenbuf, 0);
      if (i < lenbuf)
      {
          printf("Failed to send all\n");
      }
  }

  }
   close(soc);
   pthread_exit(NULL);
}
