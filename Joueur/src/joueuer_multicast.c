
#include "joueur_multicast.h"

void* recv_muilti_def(void* args){ //!!!!!!!! probleme 
    //receive by paquets not all at once cause your not sur of the size
    int multicast_sock=*((int *)args);
    char buff[218];
    char *x,*y,*id,*point,*message;
    while(1)
    {
        int r=recv(multicast_sock,buff,218,0);
        if(strncmp(buff,"GHOST",5)==0){
            x=buff+6;
            x[3]='\0';
            y=buff+10;
            y[3]='\0';
            printf("position GHOST x=%s y=%s\n",x,y);
        }
        else if(strncmp(buff,"SCORE",5)==0)
        {
            id=buff+6;
            id[8]='\0';
            point=buff+15;
            point[4]='\0';
            x=buff+20;
            x[3]='\0';
            y=buff+24;
            y[3]='\0';
            printf("score SCORE id=%s point=%s x=%s y=%s\n",id,point,x,y);
        }
        else if(strncmp(buff,"MESSA",5)==0){
            id=buff+6;
            id[8]='\0';
            message=buff+15;
            message[r-18]='\0';
            printf("message MESSA id=%s message=%s\n",id,message);
        }
    }
    
    
    
}

void abonner_multi(char * ip_multicast,char* port_multicast)
{
    int sock;
    sock=socket(PF_INET,SOCK_DGRAM,0);
    int ok=1;
    int r=setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,&ok,sizeof(ok));
    if(r!=0){
        perror("error");
        exit(1);
    }
    struct sockaddr_in address_sock;
    address_sock.sin_family=AF_INET;
    address_sock.sin_port=htons(atoi(port_multicast));
    address_sock.sin_addr.s_addr=htonl(INADDR_ANY);
    r=bind(sock,(struct sockaddr *)(&address_sock),sizeof(struct sockaddr_in));
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr=inet_addr(ip_multicast);
    mreq.imr_interface.s_addr=htonl(INADDR_ANY);
    r=setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,&mreq,sizeof(mreq));
    //we're gonna use sock to send and rcv hence it's gonna be returned
    pthread_t th ;
    pthread_create(&th,NULL,recv_muilti_def,&sock);
}

