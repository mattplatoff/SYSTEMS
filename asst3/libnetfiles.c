#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>


#define PORT_NUM 8686


int sockfd;

int netserverinit(const char* hostname){
	printf("net server init\n");
   int portno;
   struct sockaddr_in serv_addr;
   struct hostent *server;
	
   portno = PORT_NUM;
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	printf("sockfd = %d\n", sockfd);
   server = gethostbyname(hostname);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }

   printf("got server \n");
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   printf("set server params\n");
   /* Now connect to the server */
   
   if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }

   printf("exit net server init\n");
   return 0;
}

int netopen(const char* pathname, int flags){
	printf("net open called\n");
   int n,fdes;
	char* flag;
	if (flags == O_RDONLY){
		flag="r-";
	}
	else if (flags == O_WRONLY){
		flag="w-";
	}
	else if (flags == O_RDWR){
		flag="rw";
	}
	char sendBuff[1024];
	strcpy(sendBuff,"NOPEN:");
	strcat(sendBuff,pathname);
	strcat(sendBuff,":");
	strcat(sendBuff,flag);

	n = write(sockfd,sendBuff,1024);
   	
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   printf("n val was %d\n",n );
   
   /* Now read server response */
   char buffer[1024];
   bzero(buffer,1024);
   int i;
   n = read(sockfd, buffer, 1024);
   printf("%s\n",buffer);
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   //I think i should start at 6, not 5, and also am now mallocing mem for fdes string -> more effecient with memory, although it is trivial
   for(i = 6; buffer[i]!=':'; i++);
   char* num = (char*)malloc(i-5);
   strncpy(num,&buffer[6],i-6);
   num[i-5] = '\0';
   fdes = atoi(num);
   printf("%d\n", fdes);
   if (fdes==-1){
      //change this to error message later
      perror("An Error has occoured");
   }
   free(num);
   
   return fdes;
}

void netwrite(int filedes, void* buf, size_t nbyte){
	int n;
   char fdes[16];
   char size[16];
   char buffer[1024];
   sprintf(fdes,"%d:",filedes);
   sprintf(size,"%zu:",nbyte);
   strcpy(buffer,"NWRIT:");
   strcat(buffer,fdes);
   strcat(buffer,size);
   strcat(buffer,buf);

	n = write(sockfd,buffer,1024);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   char buff[256];
   bzero(buff,256);
   n = read(sockfd, buff, 255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
}

void netread(int filedes, void* buf, size_t nbyte){
	int n;
	char fdes[16];
	char size[16];
	char sendBuff[1024];

	sprintf(fdes, "%d:", filedes);
	sprintf(size, "%zu:", nbyte);
	strcpy(sendBuff, "NREAD:");
	strcat(sendBuff, fdes);
	strcat(sendBuff, size);

	n = write(sockfd,sendBuff,1024);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   char* fromServer = (char*)malloc(nbyte + 11);
   n = read(sockfd, fromServer, nbyte+11);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   int j, i;
   for(i=6;fromServer[i]!=':';i++);
   for(j=i;fromServer[j]!=':';j++);
    
   if(fromServer[6]!='0')
   {
   		perror("ERROR reading from file");
   		return;
   }

   strncpy(buf,&fromServer[j+1],nbyte);
   free(fromServer);		
}

int netclose(int fd){
	int n;

	char fdes[12];
	sprintf(fdes,"%d",fd);

	char sendBuff[1024];
	strcpy(sendBuff,"NCLOS:");
	strcat(sendBuff,fdes);
	strcat(sendBuff,":");
	n = write(sockfd,sendBuff,1024);
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   /* Now read server response */
   char buffer[256];
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   printf("%s\n",buffer);
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   if (buffer[6]!='0'){
      perror("Error closing file");
   }
   return 0;
}



int main(int argc, char const *argv[])
{
   printf("client started attempting to connect on %s\n",argv[1]);
	netserverinit(argv[1]);
	int fd = netopen("test.txt",O_RDONLY);
	netclose(fd);
	return 0;
}

