
#include "joueur_debut.h"

//demander de commencer une partie
void start(int connection_socket){
    if (send(connection_socket,"START***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }


}

//recevoir la reponse du start
int start_reply(int connection_socket,char* udp_port){
    int size=39;
    char reply[39];
    int inc=0;
    while (inc<8)
    {
        int r=recv(connection_socket,reply+inc,size-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;   
    }

    if(!strncmp(reply,"DUNNO",5)){
        printf("you are not register\n");
        return 0;
    }

    while (inc<size)
    {
        int r=recv(connection_socket,reply+inc,size-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;   
    }

    uint8_t n_partie=*((uint8_t *)(reply+6));
    uint16_t h=ntohs(*((uint16_t *)(reply+8)));
    uint16_t w=ntohs(*((uint16_t *)(reply+11)));
    uint8_t n_fantom=*((uint8_t *)(reply+14));
    char *ip=(char *)(reply+16);
    ip[15]='\0';
    char *port=(char *)(reply+32);
    port[4]='\0';
    
    char *real_ip=remove_hashtags(ip);
    printf("WELCOME in game N° %u ,hauteur %u,largeur %u, nb fontomes %u ,ip %s ,port %s\n",n_partie,h,w,n_fantom,real_ip,port);

    int size_pos=25;
    char reply_pos[25];
    inc =0;
    while (inc<size_pos)
    {
        int r=recv(connection_socket,reply_pos+inc,size_pos-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    char *id =(char *)(reply_pos+6);
    id[8]='\0';
    char *x=(char *)(reply_pos+15);
    x[3]='\0';
    char *y=(char *)(reply_pos+19);
    y[3]='\0';
    printf("POSITION for id=%s is x= %d,y= %d\n\n",id,atoi(x),atoi(y));

    //start listening to the private msg
    pthread_t th;
    pthread_create(&th,NULL,recv_udp_message,udp_port);
    abonner_multi(ip,port);
    return 1;
    
}