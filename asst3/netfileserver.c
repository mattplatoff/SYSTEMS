#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT_NUM 8686

void processConnection(int sockfd){
	printf("process connection\n");
	int n;
   	char buffer[256];
   	bzero(buffer,256);
   	n = read(sockfd,buffer,255);
   
   	if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   	}
   
   	printf("message recieved: %s\n",buffer);
   	n = write(sockfd,"I got your message",18);
   
   	if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   	}
}

// int lRead(int n, FILE* fp ){

// }

// int lopen(const char * filename, const char * mode){

// }

// int lclose(FILE* fp){

// }

// int lwrite(const char *s, FILE *fp){

// }

void openSocket(int port){
	
	int sockfd, newsockfd, portno, clilen;
   struct sockaddr_in serv_addr, cli_addr;
   int pid;
   
   /* First call to socket() function */
   sockfd = socket(AF_INET, SOCK_STREAM, 0);
   
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
   
   /* Now bind the host address using bind() call.*/
   if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
      perror("ERROR on binding");
      exit(1);
   }
   
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
	openSocket(PORT_NUM);
}