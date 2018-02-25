//
// Created by sd on 25.02.2018.
//

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

#include "netio.h"

extern int set_addr(struct sockaddr_in *addr,char *name,u_int32_t inaddr,short sin_port)
{
    struct hostent *h;
    memset((void*)addr,0, sizeof(*addr));
    addr->sin_family=AF_INET;
    if(name != NULL)
    {
        h=gethostbyname(name);
        if(h==NULL)
        {
            return -1;
        }
        addr->sin_addr.s_addr=*(u_int32_t *) h->h_addr_list[0];
    }
    else
    {
        addr->sin_addr.s_addr=htonl(inaddr);
    }
    addr->sin_port=htons(sin_port);
    return 0;
}

extern int stream_read(int sockfd,char *buf,int len)
{
    int characters_read;
    int remaining = len;
    while(remaining>0)
    {
        if(-1==(characters_read=read(sockfd,buf,remaining)))
        {
            return -1;
        }
        if(characters_read==0)
        {
            break;
        }
        remaining=remaining-characters_read;
        buf=buf+characters_read;
    }
    return len-remaining;

}

extern int stream_write(int sockfd,char *buf,int len)
{
    int characters_written;
    int remaining = len;
    while(remaining>0)
    {
        if(-1==(characters_written=write(sockfd,buf,remaining)))
        {
            return -1;
        }
        remaining=remaining-characters_written;
        buf=buf+characters_written;
    }
    return len-remaining;

}