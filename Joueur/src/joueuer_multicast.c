
#include "joueur_multicast.h"
void abonner_multi(char * ip_multicast,char* port_multicast)
{
    printf("abonner multicast\n");
    int sock=socket(PF_INET,SOCK_DGRAM,0);
    int ok=1;
    int r=setsockopt(sock,SOL_SOCKET,SO_REUSEPORT,&ok,sizeof(ok));
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
}


void recv_muilti_def(int multicast_sock) //!!!!!!!! probleme
{ 
    //receive by paquets not all at once cause your not sur of the size
    printf("recv multicast\n");
    void *buff=malloc(218);
    while (1)
    {
        int r=recv(multicast_sock,buff,218,0);
        if(strncmp((char *)(buff),"GHOST",5)==0)
        {
            char * x=malloc(3);
            strncpy(x,(char*)(buff+6),3);
            char * y=malloc(3);
            strncpy(y,(char*)(buff+6+4),3);
            printf("position GHOST x=%s y=%s\n",x,y);
            free(x);x=NULL;
            free(y);y=NULL;
            
        }

        if(strncmp((char *)(buff),"SCORE",5)==0)
        {
            char *id=malloc(8);
            strncpy(id,(char*)(buff+6),8);
            char *point=malloc(4);
            strncpy(point,(char*)(buff+6+9),4);
            char * x=malloc(3);
            strncpy(x,(char*)(buff+6+9+5),3);
            char * y=malloc(3);
            strncpy(y,(char*)(buff+6+9+5+4),3);
            printf("score SCORE id=%s point=%s x=%s y=%s\n",id,point,x,y);
            free(id);id=NULL;
            free(point);point=NULL;
            free(x);x=NULL;
            free(y);y=NULL;
            
        }
         if(strncmp((char *)(buff),"MESSA",5)==0)
        {
            char *id=malloc(8);
            strncpy(id,(char*)(buff+6),8);
            char * message=malloc(r-18);
            strncpy(message,(char*)(buff+6+9),r-18);
            printf("message MESSA id=%s message=%s\n",id,message);
            free(id);id=NULL;
            free(message);message=NULL;
            
            
        }
    }
    
    
    
}