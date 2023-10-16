#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <signal.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#define DEFAULT_PORT 2000
#define SERVER_IP "127.0.0.1"
#define LENUSERINPUT	32

int createConnection();
int clientWork(int soc);

fd_set fds;
struct timeval tv;
int FLAG = 0;

void alarm_handler(){
  FLAG = 1;
}

int main() {
  signal(SIGALRM, alarm_handler);
  while (1){
    int con = createConnection();
    if (con == -1) {
      printf("\nServer is not response. Reconnection (10 seconds)...\n");
      alarm(10);
      pause();
      if(FLAG) {
        FLAG = 0;
        continue;
      }
    } 
    else {
      printf("\nSuccesssul connection to the server\n");
      int res = clientWork(con);
      if( res == -1){
        printf("Reconnection (10 seconds)...\n");
        alarm(10);
        pause();
        if(FLAG) {
          FLAG = 0;
          continue;
        }
      }
      else if (res == 0){
        break;
      }
    }
  }
  
  return 0;
}

int createConnection() {
  int soc = socket(AF_INET, SOCK_STREAM, 0);
  if(soc < 0) {
    printf("Error: create socket\n");
    exit(0);
  }
  struct sockaddr_in serverAddr;
  serverAddr.sin_port = htons(DEFAULT_PORT);
  serverAddr.sin_family = AF_INET;
  inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr);
  int res = connect(soc, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
  if(res == -1) return res;
  else return soc;
}

int clientWork(int soc) {
  unsigned short adress;
  unsigned short reg_no;
  unsigned short num_regs;
  int s;
  int i;
  unsigned char sendbuf[256];
  unsigned char recvnbuf[256];

  printf("\n1)Read holding registers - enter read\n");
  printf("2)Exit - enter exit\n\n");

  char userinput[LENUSERINPUT];
  while(1)
  {
    printf("\ncmd >> ");
    fgets(userinput, LENUSERINPUT, stdin);
    if (strcmp(userinput, "exit\n") == 0){
      printf("\nConnection is closed\n");
      send(soc, "exit", 4, 0);
      break;
    }
    else if (strcmp(userinput, "read\n") == 0){

      printf("\nEnter modbus adress > ");
      scanf("%hd", &adress);
      printf("\nEnter number of first register > ");
      scanf("%hd", &reg_no);
      printf("\nEnter count of registers > ");
      scanf("%hd", &num_regs);

      FD_ZERO(&fds);
      tv.tv_sec = 5;
      tv.tv_usec = 0;

      /* check ready to send */
      FD_SET(soc, &fds);
      i = select(32, NULL, &fds, NULL, &tv);
      if(0)if (i<=0)
      {
        printf("select - error %d\n",i);
        break;
      }

      /* build MODBUS request */
      for (i=0;i<5;i++) sendbuf[i] = 0;
      sendbuf[5] = 6;
      sendbuf[6] = adress;
      sendbuf[7] = 3;
      sendbuf[8] = reg_no >> 8;
      sendbuf[9] = reg_no & 0xff;
      sendbuf[10] = num_regs >> 8;
      sendbuf[11] = num_regs & 0xff;

      int lenbuf = 12;

      /* send request */
      i = send(soc, sendbuf, lenbuf, 0);
      if (i < lenbuf)
      {
        printf("Failed to send all\n");
      }

      /* wait for response */
      FD_SET(soc, &fds);
      i = select(32, &fds, NULL, NULL, &tv);
      if (i <= 0)
      {
        printf("No TCP response received\n");
        close(soc);
        return -1;
      }

      /* receive response */
      i = recv(soc, recvnbuf, 256, 0);
      if (i < 9)
      {
        if (i == 0)
        {
          printf("\nServer crashed. ");
          close(soc);
          return -1;
        }
        else if(i == -1){
          printf("\nReceive error\n");
          close(soc);
          return -1;
        }
      }
      else if (i != (9+2*num_regs))
      {
        printf("\n%s\n", recvnbuf);
      }
      else
      {
        printf("\n\n");
        unsigned short reg;
        for (i=0;i<num_regs;i++)
        {
          reg = (recvnbuf[9+i+i]<<8) + recvnbuf[10+i+i];
          printf("register %d = %hd\n", reg_no, reg);
          reg_no++;
        }
      }
    }
  }

  close(soc);
  return 0;
}
