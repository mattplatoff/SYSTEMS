#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>

#include <netdb.h>
#include <netinet/in.h>


#define PORT_NUM 8686


extern int sockfd = -1;


int netopen(const char* pathname, int flags){
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

size_t netwrite(int filedes, void* buf, size_t nbyte){
	
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

size_t netread(int filedes, void* buf, size_t nbyte){
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

int netserverinit(char* hostname){
	int n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
	
   portno = PORT_NUM;
   
   /* Create a socket point */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
   if (sockfd < 0) {
      perror("ERROR opening socket");
      exit(1);
   }
	
   server = gethostbyname(argv[1]);
   
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

