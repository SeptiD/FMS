//
// Created by sd on 26.02.2018.
//


#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
