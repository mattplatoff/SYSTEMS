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
struct sockaddr_in serv_addr;

typedef struct fdNode
{
   char path[1024];
   int fd;
   struct fdNode *next;
} fdNode;

fdNode * head = NULL;

int netserverinit(const char* hostname){
   printf("net server init\n");
   int portno;
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

   printf("exit net server init\n");
   return 0;
}

int netopen(const char* pathname, int flags){
   printf("net open called\n");
   int n,fdes;
   char* flag;

   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      exit(1);
   }

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
   if (fdes==1){
      //change this to error message later
      perror("An Error has occoured");
   }
   free(num);
   
   close(sockfd);
   return fdes;
}

void netwrite(int filedes, void* buf, size_t nbyte){

   int n;
   int nb = (int)nbyte;
   int z = 0;
   char fdes[16];
   char size[16];
   sprintf(fdes,"%d:",filedes);

   while(nb > 0)
   {

   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      exit(1);
   }

   if(nb > 900) strcpy(size,"900:");
   else sprintf(size,"%d:",nb);
   char buffer[1024];

   strcpy(buffer,"NWRIT:");
   strcat(buffer,fdes);
   strcat(buffer,size);
   strncat(buffer,buf + z,900);
   printf("%s\n",buffer);

   n = write(sockfd,buffer,1024);
   
   if (n < 0) {
      perror("ERROR writing to socket");
      exit(1);
   }

   nb -= 900;
   z += 900; 
   bzero(buffer, 1024);
   
   
   /* Now read server response */
   char fromServer[1024];
   bzero(fromServer,1024);
   n = read(sockfd, fromServer, 1024);
   printf("%s\n",fromServer);

   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   int j, i;
   for(i=6;fromServer[i]!=':';i++);
   for(j=i;fromServer[j]!=':';j++);
    
   if(fromServer[6]=='-')
   {
         perror("ERROR reading from file");
         return;
   }

   close(sockfd);
   }

}

void netread(int filedes, void* buf, size_t nbyte){

   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      exit(1);
   }

   int n;
   char fdes[16];
   char size[16];
   char sendBuff[1024];

   sprintf(fdes, "%d:", filedes);
   sprintf(size, "%zu:", nbyte);
   strcpy(sendBuff, "NREAD:");
   strcat(sendBuff, fdes);
   strcat(sendBuff, size);
   printf("%s\n",sendBuff);
   n = write(sockfd,sendBuff,1024);
   
   if (n < 0) {
      perror("1ERROR writing to socket");
      exit(1);
   }
   
   /* Now read server response */
   char fromServer[nbyte + strlen(size)+7];
   n = read(sockfd, fromServer, nbyte+strlen(size)+7);

   if (n < 0) {
      perror("1ERROR reading from socket");
      exit(1);
   }

   int j, i;
   for(i=6;fromServer[i]!=':';i++);
   for(j=i;fromServer[j]!=':';j++);
    
   if(fromServer[6]=='-')
   {
         perror("ERROR reading from file");
         return;
   }

   if(fromServer[6] != '0') strncpy(buf,&fromServer[j+1],nbyte);

   close(sockfd);
}

int netclose(int fd){

   sockfd = socket(AF_INET, SOCK_STREAM, 0);

   if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
   {
      perror("ERROR connecting");
      exit(1);
   }

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
   n = read(sockfd, buffer, 256);
   printf("%s\n",buffer);
   if (n < 0) {
      perror("ERROR reading from socket");
      exit(1);
   }

   if (buffer[6]!='0'){
      perror("Error closing file");
   }

   close(sockfd);

   return 0;
}



int main(int argc, char const *argv[])
{
   printf("client started attempting to connect on %s\n",argv[1]);
   netserverinit(argv[1]);
   char* x = "qRCHVKQ2BQKRFLzRIy9VFxiKYFyWxIifa6uhnijIsBzkkFBv4WCVnW8YgZUsTMffE4whyvOEQa65iAlBpViXMSA3HruAq9QqeO15hevhyO2hu31hlxynnnpprj1yJkbt81GHcSUXNKkya2vtUkLvk6obQVtwHxYmHe9eACXpvQ0N3IMczNVQ0gDt39oAPicYYcHoPUT07UOWEoScYve1BVujDPITbYABNSvgPVrUNtUlASNRzTfNZZVytqByASzD93np2fQkoZjLlEUDYhsUW7zOKTpBzzfPRqPt1tyzsn08GUb1ggRK3hMzKEVETYBMGHiMfYOPwifKLQygRKuh7N31vJEUUmkykBALmMSTuh4VIp0LPrjWILxloJXS2GHqNI3f6E5a5Tt1XhkTX7PwJWjNecfkmtGGI9MDwFQAK1titlXt7DyTrYJhvp3S0hZHljfEWPckprQkS6GqgwGwNMDKBcZW9vgQS3e9FZo2IS2NApS5ZEYJRySohlb61Qm8LLW5BIwXGrCTtFLL6zupCHV7a5Zkx64u1l1Opjb703DCwj3aQUOumRT5u341wPrJNH9q71Bamnzaa5W5sFuBqX7fGgqkXoJRYrbL8BOVhsl1EbEwF8zTPaA1uRwIG3Bwn3tJe6zfPS4Xm9PhRRxEiXfqcWWlTVJIcsGffRMeUj0JT9F3JgzJgIbDLGxyjK8xvhli5OjVjyupfPgHmcWiZUIvPaZ0FSh9DKvPNNK0p58oV0nq5hIUh5ykz6QtjOtFhph0nh6DZs9WuwIPNJk1YSFgeCvLkTHRRWP4MXWWCKt3noT4jChWBLuerJPVLhQvi9P3U5wMMPX79ECMwx0pwrmsS6zVIRzEVmIuLSf9XT09tGzKjviHHaQgWSSewW58sbwyua9ZeYsFcf1Ay8uVeQ5XzqwSXT36WiQr61Cq2rRZshcmGwKyNghuGSn6glEC76AONo2f3XKg52RjARanDa9iyz3pmOImlkX9DtZzV0YgxpQk04ZS2xtqx01TYCMPmJ7D3BouvuYhggPYl3oL0IV5PqN7vHpIeq982kcFJ1NXwUI4ZRpMG2v2DgPGruOhfx5M6Qs9YAYY";
   //char* y = malloc(1026);
   int fd = netopen("file.txt",O_RDWR);
   netwrite(fd, x, 1100);
   //printf("%s\n", y);
   netclose(fd);
   
   return 0;
}

