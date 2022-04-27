
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
//nhi ta3k hat hadia
void start_reply(int connection_socket,char* udp_port)
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
    uint16_t h=ntohs(*((uint16_t *)(reply+6+2)));
    uint16_t w=ntohs(*((uint16_t *)(reply+6+2+3)));
    uint8_t n_fantom=*((uint8_t *)(reply+6+2+3+3));
    char *ip=malloc(15);
    strncpy(ip,(char *)(reply+6+2+3+3+2),15);
    char *port=malloc(4);
    strncpy(port,(char *)(reply+6+2+3+3+2+16),4);
    
    char *real_ip=remove_hashtags(ip);
    abonner_multi(real_ip,port);//here starts the mutlticast 
    printf("WELCOME %u ,hauteur %u,largeur %u, nb fontomes %u ,ip %s ,port %s\n",n_partie,h,w,n_fantom,real_ip,port);
    free(ip);
    ip=NULL;
    free(port);
    port=NULL;

    free(reply);
    reply=NULL;

    int size_pos=5+8+3+3+3+3;
    void *reply_pos=malloc(size_pos);
    inc =0;
    while (inc<size_pos)
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
    printf("POSITION id=%s ,x= %d,y= %d\n",id,atoi(x),atoi(y));
    free(reply_pos);
    reply_pos=NULL;

    //start listening to the private msg
    pthread_t th;
    pthread_create(&th,NULL,recv_udp_message,udp_port);
    pthread_join(th,NULL);
    
}