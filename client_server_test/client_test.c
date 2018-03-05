//==============INCLUDES==================//
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>

//==============DEFINES===================//
#define BUFF_SIZE 256 // read buffer size

//Error expressing function
void error(char *msg) {
  perror(msg);
  exit(0);
}

int main(int argc, char *argv[])
{
  int socket_fd;
  int port_number;
  int message_status;
  int connection_status;
  struct sockaddr_in server_address;
  struct hostent *server;

  char buffer[BUFF_SIZE];

  //Verifying number of arguments
  if (argc < 3) {
      fprintf(stderr,"Usage %s hostname port\n", argv[0]);
      exit(0);
    }

  ////Port number is being given as an argument
  port_number = atoi(argv[2]);

  //Creating the socket
  //int socket (int family, int type, int protocol);
  socket_fd = socket( AF_INET, SOCK_STREAM, 0);

  //Verifying socket creation
  if (socket_fd < 0) {
    error("ERROR opening socket");
  }

  //Getting the host by 127.0.0.1 name given as an argument
  server = gethostbyname(argv[1]);

  //Verifying that the host is valid
  if (server == NULL) {
    fprintf(stderr, "ERROR, no such host\n");
    exit(0);
  }

  //Setting the server address to 00
  //bzero ( char *dest, int nbytes);
  bzero( (char *)&server_address, sizeof(server_address));

  //Setting the family 
  server_address.sin_family = AF_INET;

  //bcopy (char *src, char *dest, int nbytes);
  bcopy( (char *)server->h_addr, (char *)&server_address.sin_addr, server->h_length);

  //Setting the port number
  server_address.sin_port = htons(port_number);
  
  //Connecting to the server
  //int connect ( int sockfd, struct sockaddr *myaddr, int addrlen);
  connection_status = connect(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));

  //verifying if connection worked
  if ( connection_status < 0) {
      error("ERROR connecting");
    }

  //For this example we enter a message and display it on the server
  printf("Please enter the message: ");

  //Reset the buffer
  //bzero ( char *dest, int nbytes);
  bzero(buffer, BUFF_SIZE);

  //Read the message
  //char fgets (char * restrict str, int size, FILE * restrict stream)
  fgets(buffer, BUFF_SIZE-1, stdin);

  //Write the message
  message_status = write(socket_fd, buffer, strlen(buffer));

  //Verify if the message was sent properly
  if (message_status < 0) {
      error("ERROR writing to socket");
    }

  //Reset the buffer
  //bzero ( char *dest, int nbytes);
  bzero(buffer,BUFF_SIZE);

  //Read the buffer
  message_status = read(socket_fd, buffer, BUFF_SIZE-1);

  //Verify if the read was made properly
  if (message_status < 0) {
      error("ERROR reading from socket");
    }

  //Print the readed message
  printf("%s\n", buffer);

  return 0;
}
