#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

void netopen(const char* pathname, int flags){
	int n;
	n = write(sockfd,"NOPEN",strlen("NOPEN"));
   
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

void netwrite(int filedes, void* buf, size_t nbyte){
	int n;
	n = write(sockfd,"NWRITE",strlen("NWRITE"));
   
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

void netclose(int fd){
	int n;
	n = write(sockfd,"NCLOSE",strlen("NCLOSE"));
   
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



int main(int argc, char const *argv[])
{
	netserverinit(argv[1]);
	netopen("",-1);
	return 0;
}

