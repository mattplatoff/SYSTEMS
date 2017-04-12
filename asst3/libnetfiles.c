#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>


#define PORT_NUM 8686


int sockfd = -1;

int netserverinit(const char* hostname){
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
	
   server = gethostbyname(hostname);
   
   if (server == NULL) {
      fprintf(stderr,"ERROR, no such host\n");
      exit(0);
   }
   
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(portno);
   
   /* Now connect to the server */
   if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR connecting");
      exit(1);
   }
   
   return 0;
}

int netopen(const char* pathname, int flags){
	int n,fdes;
	char flag[3];
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

   
   /* Now read server response */
   char buffer[1024];
   bzero(buffer,1024);
   int i;
   n = read(sockfd, buffer, 1024);

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
   if (fdes==-1){
      //change this to error message later
      perror(An Error has occoured);
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
   sprintf(size,"%d:",nbyte);
   strcpy(buffer,"NWRIT:");
   strcat(buffer,fdes);
   strcat(buffer,size);
   strcat(buffer,buf);

	n = write(sockfd,"NWRIT",strlen("NWRIT"));
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   char buffer[256];
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
}

void netread(int filedes, void* buf, size_t nbyte){
	int n;
	n = write(sockfd,"NREAD",strlen("NREAD"));
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   char buffer[256];
   bzero(buffer,256);
   n = read(sockfd, buffer, 255);
   
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
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
   
   int i;
   if (buffer[6]!='0'){
      perror("Error closing file");
   }

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }
}



int main(int argc, char const *argv[])
{
	netserverinit(argv[1]);
	netopen("",-1);
	return 0;
}

