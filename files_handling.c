//
// Created by sd on 26.02.2018.
//


#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#define BUF_SIZE 256
void create_directory_structure_file(char *file_name,char *path_to_folder)
{
    pid_t myPid;
    myPid = fork();
    if(myPid == 0)
    {
        int fd = open("file.txt", O_CREAT | O_WRONLY, 0777);
        dup2(fd,1);
        execlp("sh","sh","makeTree",path_to_folder,file_name, NULL);
        puts("you should not be here\n");
    }
}

int write_file_to_socket(char *file_name,int socket_fd)
{
    int fd;
    int file_read=0;
    char buf[BUF_SIZE];
    if((fd=open(file_name,O_RDONLY))<0)
    {
        return -1;
    }

    while((file_read=read(fd,buf,BUF_SIZE-1))>0)
    {

        write(socket_fd,buf,file_read);
    }

    if(file_read==-1)
    {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}


int read_file_from_socket(char *file_name,int socket_fd)
{
    int fd;
    int file_read;
    char buf[BUF_SIZE];
    if((fd=creat(file_name,S_IRUSR|S_IWUSR))<0)
    {
        return -1;
    }

    while((file_read=read(socket_fd,buf,BUF_SIZE-1))>0)
    {

        write(fd,buf,file_read);
    }

    if(file_read==-1)
    {
        close(fd);
        return -1;
    }
    close(fd);
    return 0;
}
