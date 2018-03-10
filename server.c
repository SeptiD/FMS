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
#include <string.h>

#include "files_handling.h"
#include "codes.h"

//==============DEFINES===================//
#define BUFF_SIZE 256 // read buffer size
#define PORT_NUMBER 5647
#define SERVER_TREE_FILE_NAME "serverTreeFile"
#define SERVER_TREE_PATH "/home/sd/Desktop/FMS-copy"
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
    pid_t pid;

    create_directory_structure_file(SERVER_TREE_FILE_NAME,SERVER_TREE_PATH);

    //Verifying number of arguments
    /*
    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided\n");
        exit(1);
    }*/

    //Port number is being given as an argument
    port_number = PORT_NUMBER;

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
    while(1) {
        //We wait for a client to request, and then we will accept it's request
        //int accept (int sockfd, struct sockaddr *peer, int *addrlen);
        new_socket_fd = accept(socket_fd, (struct sockaddr *) &client_address, &client_address_length);

        //Verifying if the accept function worked properly
        if (new_socket_fd < 0) {
            error("ERROR on accept");
        }
        pid = fork();
        switch (pid) {
            case -1:
                printf("pid error\n");
                break;
            case 0://child process
                close(socket_fd);

                //start the communication
                //step 1 ->establish connection

                bzero(buffer, BUFF_SIZE);
                read_write_status = read(new_socket_fd, buffer, BUFF_SIZE - 1);
                if (read_write_status < 0) {
                    error("ERROR reading from socket");
                }

                print_action(buffer[0]-'0');
                if((buffer[0]-'1')!=0)
                {
                    //printf("aici\n");
                    print_action(ERROR);
                    exit(-1);
                }

                bzero(buffer, BUFF_SIZE);
                snprintf(buffer,2,"%d",EST_CON_ACK);
                read_write_status = write(new_socket_fd,buffer ,strlen(buffer));
                print_action(EST_CON_ACK);
                if (read_write_status < 0) {
                    error("ERROR writing to socket");
                }

                //step 2 -> send the tree file
                bzero(buffer, BUFF_SIZE);
                read_write_status = read(new_socket_fd, buffer,  1);
                if (read_write_status < 0) {
                    error("ERROR reading from socket");
                }

                print_action(buffer[0]-'0');
                if((buffer[0]-'0')!=REQ_TREE_FILE)
                {
                    print_action(ERROR);
                    exit(-1);
                }



                bzero(buffer, BUFF_SIZE);
                snprintf(buffer,2,"%d",REQ_TREE_FILE_ACK);
                read_write_status = write(new_socket_fd,buffer ,strlen(buffer));
                print_action(REQ_TREE_FILE_ACK);
                if (read_write_status < 0) {
                    error("ERROR writing to socket");
                }

                if((write_file_to_socket(SERVER_TREE_FILE_NAME,new_socket_fd))!=0)
                {
                    print_action(ERROR);
                    exit(-1);
                }


                exit(0);
            default://parent process
                close(new_socket_fd);

        }

    }
    return 0;
}


