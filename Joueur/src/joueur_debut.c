
#include "joueur_debut.h"

void start(int connection_socket)
{
    printf("start \n");
    if (send(connection_socket,"START***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }


}

void start_reply(int connection_socket)
{
    printf("start reply \n");
    int size=5+1+2+2+1+15+4+3+6;
    void * reply=malloc(size);
    int inc=0;
    while (inc<size)
    {
        int r=recv(connection_socket,reply+inc,size-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(reply);
            reply=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    uint8_t n_partie=*((uint8_t *)(reply+6));
    uint16_t h=*((uint16_t *)(reply+6+2));
    uint16_t w=*((uint16_t *)(reply+6+2+3));
    uint8_t n_fantom=*((uint8_t *)(reply+6+2+3+3));
    char *ip=malloc(15);
    strncpy(ip,(char *)(reply+6+2+3+3+2),15);
    char *port=malloc(4);//to be returnd
    strncpy(ip,(char *)(reply+6+2+3+3+2+16),4);
    
    char *real_ip=remove_hashtags(ip);// to be returned 

    free(ip);
    ip=NULL;
    free(port);
    port=NULL;

    free(reply);
    reply=NULL;

    int size_pos=5+8+3+3+3+3;
    void *reply_pos=malloc(size_pos);
    inc =0;
    while (inc<size)
    {
        int r=recv(connection_socket,reply_pos+inc,size_pos-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(reply_pos);
            reply_pos=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    char *id =malloc(8);
    strncpy(id,(char *)(reply_pos+6),8);
    char *x=malloc(3);
    strncpy(x,(char *)(reply_pos+6+9),3);
    char *y=malloc(3);
    strncpy(y,(char *)(reply_pos+6+9+4),3);
    
    free(reply_pos);
    reply_pos=NULL;
    
}