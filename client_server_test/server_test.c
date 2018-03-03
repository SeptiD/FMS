//==============INCLUDES==================//
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

//==============DEFINES===================//
#define BUFF_SIZE 256 // read buffer size

//Error expressing function
void error(char *msg) {
  perror(msg);
  exit(1);
}

int main(int argc, char *argv[]) {
  int socket_fd;
  int new_socket_fd;
  int port_number;
  int read_write_status;
  int bind_return;
  int no_of_hold_requests;
  unsigned int client_address_length;
  char buffer[BUFF_SIZE]; 
  struct sockaddr_in server_address;
  struct sockaddr_in client_address;

  //Verifying number of arguments
  if (argc < 2) {
      fprintf(stderr,"ERROR, no port provided\n");
      exit(1);
  }

  //Port number is being given as an argument
  port_number = atoi(argv[1]);

  //Creating the socket
  //int socket (int family, int type, int protocol);
  socket_fd = socket(AF_INET, SOCK_STREAM, 0);

  //Verifying socket creation
  if (socket_fd < 0) {
      error("ERROR opening socket");
  }

  //Setting the server address to 0000
  //bzero ( char *dest, int nbytes);
  bzero( (char *)&server_address, sizeof(server_address) );
  
  /*Structures used below
  struct sockaddr {
     u_short sa_family; //address family 
     char  sa_data[14]; // up to 14 bytes of protocol specific address 
  };

  struct in_addr {
     u_long s_addr;  // 32-bit netid/hostid 
  };
  
  struct sockaddr_in {
    short sin_family;  // AF_INET  
    u_short sin_port;  // 16-bit port number  
    struct in_addr sin_addr; // 32-bit netid/hostid 
    char sin_zero[8]; // unused  
	};*/

  //Set the server sockaddr_in struct
  server_address.sin_family = AF_INET; //setting the family
  server_address.sin_port = htons(port_number); //setting the port number
  server_address.sin_addr.s_addr = INADDR_ANY; //for binding the server to all it's available interfaces, not just to localhost 

  //Binding the created socket with the set server
  //int bind (int sockfd, struct sockaddr *myaddr, int addrlen);
  bind_return = bind(socket_fd, (struct sockaddr *)&server_address, sizeof(server_address));

  //Verifying binding operation 
  if ( bind_return < 0) {
      error("ERROR on binding");
  }

  //Making the server to be able to hold up to no_of_hold_requests requests to the server
  //int listen ( int sockfd, int backlog);
  no_of_hold_requests = 5; // usually the backlog argument is 5
  listen(socket_fd,no_of_hold_requests);

  //Taking the length of the client's address
  client_address_length = sizeof(client_address);

  //We wait for a client to request, and then we will accept it's request
  //int accept (int sockfd, struct sockaddr *peer, int *addrlen);
  new_socket_fd = accept(socket_fd, (struct sockaddr *)&client_address, &client_address_length);

  //Verifying if the accept function worked properly
  if (new_socket_fd < 0) {
      error("ERROR on accept");
  }

  //Setting the buffer to 000...00
  //bzero ( char *dest, int nbytes);
  bzero(buffer,BUFF_SIZE);

  //Making a read operation
  read_write_status = read(new_socket_fd, buffer, BUFF_SIZE-1 );

  //Verifying the read status
  if (read_write_status < 0) {
      error("ERROR reading from socket");
  }

  //Printing the received message
  printf("Here is the message: %s\n",buffer);

  //Making a write operation
  read_write_status = write(new_socket_fd,"I got your message",18);

  //Verifying the write status
  if (read_write_status < 0) {
      error("ERROR writing to socket");
  }
  return 0;
}


