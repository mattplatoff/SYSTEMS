#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>

#define PORT_NUM 8686
#define UNRESTRICTED 0
#define EXCLUSIVE 1
#define TRANSACTION 2

typedef struct fdData
{
  int fd;
  int mode;
  struct fdData * next;
} fdData;

fdData * head = NULL;

int lRead(char* buff, char* readTo, int size){
  int fdes, i, j, n;
  for(i=6;buff[i]!=':';i++);
    i++;
  for(j=i;buff[j]!=':';j++);
  char* fd = (char*)malloc(i-5);
  printf("--%s,%d\n",buff,i);
  strncpy(fd, &buff[6], i-6);
  fd[i-6] = '\0';
  fdes = atoi(fd)*-1;
  free(fd);

  n = read(fdes, readTo, size);
  readTo[size] = '\0';

  return n;

}

int lopen(char * buff){
  printf("local open\n");
  char path[1024];
  char flag[3];
  char mode[2];
  int i, j, iflag, fdes, networkfdes, imode;

  //pull params out of buffer 
  for (i=6;buff[i]!=':';i++);
    printf("i: %d\n", i);
  memcpy(path,&buff[6],i-6);
  path[i-6] = '\0';
  memcpy(flag,&buff[i+1],2);
  flag[2] = '\0';

  i++;  
  for (j=i;buff[j]!=':';j++);
  strncpy(mode,&buff[j+1],1);
  mode[1] = '\0';
  imode = atoi(mode);

  printf("path: %s\n",path);
  printf("flag: %s\n",flag);
  printf("mode: %d\n",imode);

  if (!strncmp(flag,"r-",2)){
    iflag=O_RDONLY;
  }
  else if (!strncmp(flag,"w-",2)){
    iflag = O_WRONLY;
  }
  else if(!strncmp(flag,"rw",2)){
    iflag = O_RDWR;
  }
   else{
      iflag=0;
   }
  //open file and return file discriptor 
  fdes = open(path, iflag);
   networkfdes = fdes*-1;

  return networkfdes;
}

int lclose(char* buff){
  printf("local close\n");
  int i, fdes, n;

  for(i = 6; buff[i]!=':'; i++);
    char* num = (char*)malloc(i-5);
    strncpy(num,&buff[6],i-6);
    num[i-6] = '\0';
    fdes = atoi(num) * -1;
    free(num);

    n = close(fdes);
    printf("%d\n",n);

  return n;
}

int lwrite(const char *buff){
  char fdes[16];
   char size [16];
   char message[1024];
   int fdesc,wsize;
   int i,j,ret;
   for (i=6;buff[i]!=':';i++);
   i++;
   strncpy(fdes,&buff[6],i-6);
   fdesc=atoi(fdes);
   for (j=i;buff[j]!=':';j++);
   strncpy(size,&buff[i],j-i);
   wsize = atoi(size);
   fdesc*=-1;
   strcpy(message,&buff[j+1]);
      printf("fdes: %d\n",fdesc);

   ret = write(fdesc,message,wsize);
   printf("%d\n",ret);
   perror("hi");

   return ret;
}

void* processConnection(void* fd){

   int sockfd = *(int*)fd;
   printf("connection established\n");
   int n; //,end = 0;
      char buffer[1024];
      char retBuff[1024];


        bzero(buffer,1024);
        n = read(sockfd,buffer,1024);
   
        if (n < 0) {
       perror("ERROR reading from socket");
       exit(1);
       }
   
        printf("message recieved: %s\n",buffer);

        if (!strncmp(buffer,"NOPEN",5)){
           printf("NOPEN\n");
           //get local file descriptor, should then save this and send back dummy file descriptor? and keep relationship
           int fdes = -1;
           //pass in buffer from client and open with params 
          fdes=lopen(buffer);
          //prepare to send back fdes, make up own fdes tho and store relationship.
          // as per program spec, fd on client side should be negative of fd on server side
          char num[12];
          bzero(retBuff,1024);
          strcpy(retBuff,"NOPEN:");
          printf("hhhhh %d\n", fdes);
          if(fdes < 0) sprintf(num,"%d",fdes);
          else sprintf(num,"%d",errno);
          strcat(retBuff,num);
          strcat(retBuff,":");

          printf("%s\n",retBuff);
           //write back to client
          n = write(sockfd,retBuff,1024);
           if (n < 0) {
           perror("ERROR writing to socket");
          exit(1);
          }
       }

        else if (!strncmp(buffer,"NCLOS",5)){
           printf("NCLOS\n");

          //0 if success, -1 if failure
          int success = lclose(buffer);
          char succ[8];
          bzero(retBuff,1024);
          strcpy(retBuff,"NCLOS:");
          if(success == 0) sprintf(succ,"%d:",success);
          else sprintf(succ,"%d:",errno);
          strcat(retBuff,succ);
          //write back to client
          n = write(sockfd,retBuff,1024);
          printf("%s\n", retBuff);
          if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
          }
        }
        else if (!strncmp(buffer,"NWRIT",5)){
          printf("NWRIT\n");
          int success = lwrite(buffer);
          char succ[8];
          bzero(retBuff,1024);
          strcpy(retBuff,"NWRIT:");
          if (success >= 0) sprintf(succ,"%d:",success);
          else sprintf(succ,"-%d:",errno);
          strcat(retBuff,succ);

          n = write(sockfd,retBuff,1024);
          if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
          }
        }
        else if (!strncmp(buffer,"NREAD",5)){
          int size, i, j;
          for(i=6;buffer[i]!=':';i++);
          i++;
          for(j=i;buffer[j]!=':';j++);
          char* s = (char*)malloc((j-i) + 1);
          strncpy(s, &buffer[i], j-i);
          s[(j-i)] = '\0';
          size = atoi(s);

          char* readTo = (char*)malloc(size + 1);
          int success = lRead(buffer,readTo, size);
          char succ[8];
          if(success >= 0) sprintf(succ,"%d:",success);
          else sprintf(succ,"-%d:",errno);
          char sendToClient[strlen(readTo)+strlen(succ)+7];
          strcpy(sendToClient,"NREAD:");
          strcat(sendToClient,succ);
          if(success > 0) strcat(sendToClient,readTo);
          //write back to client
          n = write(sockfd,sendToClient, strlen(sendToClient)+1);
          if (n < 0) {
            perror("ERROR writing to socket");
            exit(1);
          }

          free(readTo);
          free(s);
        }

      close(sockfd);  
      return NULL;
}

void openSocket(int port){
  printf("open socket\n");
  int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   printf("sockfd = %d\n",sockfd );
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }

   if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0)
    perror("setsockopt(SO_REUSEADDR) failed");
   
   /* Initialize socket structure */
   bzero((char *) &serv_addr, sizeof(serv_addr));
   portno = PORT_NUM;
   serv_addr.sin_family = AF_INET;
   serv_addr.sin_addr.s_addr = INADDR_ANY;
   serv_addr.sin_port = htons(portno);
   
   printf("bind\n");
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   printf("after bind\n");
   /* Now start listening for the clients, here
      * process will go in sleep mode and will wait
      * for the incoming connection
   */
   
   listen(sockfd,5);
   clilen = sizeof(cli_addr);
   
   while (1) {
      int* sockptr = malloc(sizeof(int));
      puts("test");
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);

      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }

      *sockptr = newsockfd;
      pthread_t thread0;  
      void* (*procCon)(void*) = processConnection;

      int err = pthread_create(&thread0, NULL, procCon, (void*) sockptr);
      if(err != 0)
      {
        perror("ERROR on thread creation");
        exit(1);
      }

      pthread_detach(thread0);
      //close(newsockfd);

    }

    close(sockfd);
    /*
    
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
      
      // Create child process 
      pid = fork();
    
      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
      
      if (pid == 0) {
         // This is the client process 
         close(sockfd);
         processConnection(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
    */
}

int main(int argc,char** argv ){
  printf("server started\n");
   openSocket(PORT_NUM);
   return 0;
}