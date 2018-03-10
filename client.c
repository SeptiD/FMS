//
// Created by sd on 05.03.2018.
//

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

#include "files_handling.h"
#include "codes.h"

//==============DEFINES===================//
#define BUFF_SIZE 256 // read buffer size
#define PORT_NUMBER 5647
#define SERVER_ADDRESS "127.0.0.1"
#define SERVER_TREE_FILE_NAME "ServerTreeFileToClient"

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

    ////Port number is being given as an argument
    port_number = PORT_NUMBER;

    //Creating the socket
    //int socket (int family, int type, int protocol);
    socket_fd = socket( AF_INET, SOCK_STREAM, 0);

    //Verifying socket creation
    if (socket_fd < 0) {
        error("ERROR opening socket");
    }

    //Getting the host by 127.0.0.1 name given as an argument
    server = gethostbyname(SERVER_ADDRESS);

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

    //start the communication
    //step 1 ->establish connection

    bzero(buffer, BUFF_SIZE);
    snprintf(buffer,2,"%d",EST_CON);
    message_status = write(socket_fd, buffer, strlen(buffer));
    print_action(EST_CON);

    if (message_status < 0) {
        error("ERROR writing to socket");
    }


    bzero(buffer,BUFF_SIZE);
    message_status = read(socket_fd, buffer, BUFF_SIZE-1);

    if (message_status < 0) {
        error("ERROR reading from socket");
    }

    if(buffer[0]=='2')
    {
        // printf("aici \n");
        print_action(EST_CON_ACK);
    }
    else
    {
        print_action(ERROR);
        return -1;
    }

    //step 2 -> ask for tree file
    bzero(buffer, BUFF_SIZE);
    snprintf(buffer,2,"%d",REQ_TREE_FILE);
    message_status = write(socket_fd, buffer, strlen(buffer));
    print_action(REQ_TREE_FILE);

    bzero(buffer,BUFF_SIZE);
    message_status = read(socket_fd, buffer, 1);

    if (message_status < 0) {
        error("ERROR reading from socket");
    }

    if(buffer[0]=='6')
    {
        print_action(REQ_TREE_FILE_ACK);
    }
    else
    {
        print_action(ERROR);
        return -1;
    }
    read_file_from_socket(SERVER_TREE_FILE_NAME,socket_fd);

    return 0;
}
