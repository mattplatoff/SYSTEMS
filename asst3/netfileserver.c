#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT_NUM 8686



int lread(char* buff, char* readTo){
	int fdes, size, i, j, n;

	for(i=6;buff[i]!=':';i++);
	for(j=i;buff[j]!=':';j++);
	char* fd = (char*)malloc(i-5);
	char* s = (char*)malloc((j-i) + 1);
	strncpy(fd, &buff[6], i-6);
	strncpy(s, &buff[i], j-i);
	fd[i-5] = '\0';
	s[(j-i)+1] = '\0';
	fdes = atoi(fd)*-1;
	size = atoi(s) + 1;
	free(fd);
	free(s);

	readTo = (char*)malloc(size);
	n = read(fdes, readTo, size);
	readTo[size] = '\0';

	return n;

}

int lopen(char * buff){
	printf("local open\n");
	char path[1024];
	char flag[3];
	int i, iflag, fdes, networkfdes;

	//pull params out of buffer 
	for (i=6;buff[i]!=':';i++)
      ;
	memcpy(path,&buff[6],i-5);
	path[i-5] = '\0';
	memcpy(flag,&buff[i+1],2);
	flag[2] = '\0';

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

	for(i = 6; buff[i]!=':'; i++)
      ;
   	char* num = (char*)malloc(i-5);
   	strncpy(num,&buff[6],i-6);
   	num[i-5] = '\0';
   	fdes = atoi(num) * -1;
   	free(num);

   	n = close(fdes);

	return n;
}

int lwrite(const char *buff){
	char fdes[16];
   char size [16];
   char message[1024];
   int fdesc,wsize;
   int i,j,ret;
   for (i=6;buff[i]!=':';i++)
      ;
      strncpy(fdes,&buff[i],i-6);
   for (j=i+1;buff[j]!=':';j++)
      ;
      strncpy(size,&buff[i],j-i);
   wsize = atoi(size);
   fdesc=atoi(fdes);
   fdesc*=-1;
   strncpy(message,&buff[j+1],wsize);
   ret = write(fdesc,message,wsize);


   return ret;
}

void processConnection(int sockfd){
   printf("connection established\n");
   int n;
      char buffer[1024];
      char retBuff[1024];

      bzero(buffer,1024);
      n = read(sockfd,buffer,1023);
   
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
         fdes*=-1;
         char num[12];
         bzero(retBuff,1024);
         strcpy(retBuff,"NOPEN:");
         sprintf(num,"%d",fdes);
         strcat(retBuff,num);
         strcat(retBuff,":");

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
         char succ[4];
         bzero(retBuff,1024);
         strcpy(retBuff,"NCLOS:");
         sprintf(succ,"%d:",success);
         strcat(retBuff,succ);

         //write back to client
         n = write(sockfd,retBuff,1024);
         if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
         }
      }
      else if (!strncmp(buffer,"NWRIT",5)){
         lwrite(buffer);
      }
      else if (!strncmp(buffer,"NREAD",5)){
      	char* readTo = (char*)malloc(1024*sizeof(char));
      	int success = lread(buffer,readTo);

      	char succ[4];
        sprintf(succ,"%d:",success);
        char* sendToClient = (char*)malloc(strlen(readTo) + strlen(succ) + 7);
        strcpy(sendToClient,"NREAD:");
        strcat(sendToClient,succ);
        strcat(sendToClient,readTo);
      	//write back to client
      	n = write(sockfd,sendToClient, strlen(sendToClient));
        if (n < 0) {
         perror("ERROR writing to socket");
         exit(1);
        } 
        free(sendToClient);
        free(readTo);   	
      }

      n = write(sockfd,"I got your message",18);
   
      if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
      }
}

void openSocket(int port){
	printf("open socket\n");
	int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   int pid;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   printf("sockfd = %d\n",sockfd );
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
   
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
      newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *)&clilen);
		
      if (newsockfd < 0) {
         perror("ERROR on accept");
         exit(1);
      }
      
      /* Create child process */
      pid = fork();
		
      if (pid < 0) {
         perror("ERROR on fork");
         exit(1);
      }
      
      if (pid == 0) {
         /* This is the client process */
         close(sockfd);
         processConnection(newsockfd);
         exit(0);
      }
      else {
         close(newsockfd);
      }
		
   } /* end of while */
}

int main(int argc,char** argv ){
	printf("server started\n");
   openSocket(PORT_NUM);
}