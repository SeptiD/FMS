//
// Created by sd on 26.02.2018.
//


#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

//#include "files_handling.h"
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

int check_regular_file(char *path,char *line)
{
    char relative_path[100];
    const char s[5] = " \n\t/";
    char *token;
    char *first_number = NULL;
    char *file_size = NULL;
    char *date = NULL;
    char *hour = NULL;
    char *file_name = NULL;

    /* get the first token */
    token = strtok(line, s);


    /* walk through other tokens */
    while( token != NULL ) {

        if(first_number == NULL)
        {
            first_number=(char*)malloc(strlen(token)*sizeof(char));
            strcpy(first_number,token);
            //printf(" %s\n", first_number);

        }else if(file_size == NULL)
        {
            file_size=(char*)malloc(strlen(token)*sizeof(char));
            strcpy(file_size,token);
            //printf(" %s\n", file_size);
        }
        else if(date == NULL)
        {
            date=(char*)malloc(strlen(token)*sizeof(char)+6);
            strcpy(date,token);
            //printf(" %s\n", date);
        }
        else if(hour == NULL)
        {
            hour=(char*)malloc(strlen(token)*sizeof(char));
            strcpy(hour,token);
            //printf(" %s\n", hour);
        }
        else if(file_name == NULL)
        {
            file_name=(char*)malloc(strlen(token)*sizeof(char));
            strcpy(file_name,token);
            printf(" %s\n", file_name);
        }
        token = strtok(NULL, s);
    }

    //now i have stored in my variables information about the current file
    FILE *current_file;
    strcpy(relative_path,path);
    strcat(relative_path,"/");
    strcat(relative_path,file_name);
    current_file  = fopen(relative_path ,"r+");
    //file doesn not exist
    if(current_file==NULL)
    {
        //we create it
        creat (relative_path,ACCESSPERMS );
        //get_file(relative_path)
    }
    else //file already exists
    {
        struct stat *myStat;
        myStat = (struct stat *)malloc(sizeof(struct stat));
        int err;
        if((err = stat(relative_path,myStat))<0)
        {
            printf("stat err \n");
            return -1;
        }

        //check size matching
        if(atoi(file_size)!=(int)(myStat->st_size))
        {
            //file was modified
            //we need to delete it, create it and bring its data from the Server
            printf("file size is %d and it should be %d \n",(int)(myStat->st_size),atoi(file_size));
            if((err = unlink(relative_path))<0)
            {
                printf("unlink err \n");
                return -1;
            }

            creat(relative_path,ACCESSPERMS);
            //get_file(relative_path)

        }else{//check date of modification to be newer in client
            char client_date[17];
            strftime(client_date, 17, "%Y-%m-%d %H:%M", localtime(&(myStat->st_ctime)));
            printf("file is %s , from server %s %s \n",client_date,date,hour);
            strcat(date," ");
            strcat(date,hour);
            int i=0;
            while(i<17)
            {
                if(client_date[i]==date[i])
                {
                    i++;
                }
                else if(client_date[i]<date[i])
                {
                    //file is not updated
                    if((err = unlink(relative_path))<0)
                    {
                        printf("unlink err \n");
                        return -1;
                    }

                    //printf("aici %s\n",relative_path);
                    creat(relative_path,ACCESSPERMS);
                    //get_file(relative_path)
                    break;
                }
                else
                {
                    //is already updated
                    break;
                }

            }
        }
    }

    return 0;
}

int process_tree_file(char *tree_file_name)
{
    FILE *f;
    char line[100];
    int len_read=0;
    f = fopen(tree_file_name,"r+");
    if(f==NULL)
    {
        return -1;
    }
    int path_starter= -1;
    char *path;
    while (fgets(line,100,f)!=NULL)
    {
        //folder line
        //example: /home/sd/Desktop/FMS-copy/client_server_test:
        if(path_starter == -1)
        {
            int i,j;
            for(i=strlen(line)-1;line[i]!='/';i--)
            {}
            path_starter = i;
        }
        len_read = strlen(line);
        path=(char*)malloc(sizeof(char)*(len_read-path_starter));
        strcpy(path,line+path_starter+1);
        int k;
        for(k=strlen(path)-1;path[k]!=':';k--)
        {}
        path[k]='\0';
        DIR *dirul;
        dirul = opendir(path);
        if(dirul != NULL)
        {
            closedir(dirul);
        }
        else
        {
            //folder doesn't exit, we create it
            if(-1==(mkdir(path, ACCESSPERMS)))
            {
                return -1;
            }
        }

        len_read=0;
        fgets(line,100,f);//read the insignificant line

        len_read=0;
        while (fgets(line,100,f)!=NULL)
        {
            if(line[0]=='\n'||line[0]=='\r'||line[0]==' '|| line[0]=='\0')
            {
                break;
            }
            else if(line[strlen(line)-2]=='/')
            {
                //if i have a folder entry in the list I ignore it as it will be processed later
                //example:  3  4096 2018-02-25 19:53 cmake-build-debug/
                continue;
            }
            //regular files
            //example: 1 3570 2018-03-03 19:05 server_test.c
            if(check_regular_file(path,line)!=0)
            {
                return -1;
            }
        }
    }
    fclose(f);
    return(0);
}