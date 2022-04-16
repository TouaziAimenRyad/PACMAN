
#include "../includes/joueur_multicast.h"
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
    printf("abonner multicast\n");
    char * res=malloc(5);
    int inc =0;
    while (inc<5)
    {
        int r=recv(multicast_sock,res+inc,5-inc,0);
        if (r==-1)
        {      
            perror("prolem while receiving\n");
            free(res);
            res=NULL;
            close(multicast_sock);
            exit(1);
        }
        inc+=r;    
    }
    if(strcmp(res,"GHOST")==0)
    {
        void * ghost=malloc(11);
        inc =0;
        while (inc<11)
        {
            int r=recv(multicast_sock,ghost+inc,11-inc,0);
            if (r==-1)
            {      
                perror("prolem while receiving\n");
                free(res);
                res=NULL;
                free(ghost);
                ghost=NULL;
                close(multicast_sock);
                exit(1);
            }
            inc+=r;    
        }
        char *x=malloc(3);
        char*y=malloc(3);
        strncpy(x,(char*)(ghost+1),3);
        strncpy(y,(char*)(ghost+1+4),3);
        free(x);x=NULL;
        free(y);y=NULL;
        free(ghost);ghost=NULL;
    }
    if (strcmp(res,"SCORE")==0)
    {   
        int size=5+8+4+3+3+3+4;
        char * score=malloc(size);
        inc =0;
        while (inc<size)
        {
            int r=recv(multicast_sock,score+inc,size-inc,0);
            if (r==-1)
            {      
                perror("prolem while receiving\n");
                free(res);
                res=NULL;
                free(score);
                res=score;
                close(multicast_sock);
                exit(1);
            }
            inc+=r;    
        }

        char * id =malloc(8);
        char *point=malloc(4);
        char *x=malloc(3);
        char*y=malloc(3);
        strncpy(id,score+1,8); 
        strncpy(point,score+1+9,4); 
        strncpy(y,score+1+9+5,3);
        strncpy(y,score+1+9+5+4,3); 
        free(x);x=NULL;
        free(y);y=NULL;
        free(point);point=NULL;
        free(id);id=NULL;
        free(score);score=NULL;
        

    }
    if (strcmp(res,"MESSA"))
    {   
        int size=200+8+3+2;
        void * mesg=malloc(size);
        inc =0;
        while (inc<size)
        {
            int r=recv(multicast_sock,mesg+inc,size-inc,0);
            if (r==-1)
            {      
                perror("prolem while receiving\n");
                free(res);
                res=NULL;
                free(mesg);
                mesg=NULL;
                close(multicast_sock);
                exit(1);
            }
            inc+=r;    
        }
        char * id =malloc(8);
        char* message=malloc(200);
        strncpy(id,mesg+1,8); 
        strncpy(message,mesg+1+9,200); 

        free(message);message=NULL;
        free(id);id=NULL;
        free(mesg);mesg=NULL;

    }
    if (strcmp(res,"ENDGA"))
    {
        int size=8+4+3+2;
        void *end=malloc(size);
        inc =0;
        while (inc<size)
        {
            int r=recv(multicast_sock,end+inc,size-inc,0);
            if (r==-1)
            {      
                perror("prolem while receiving\n");
                free(res);
                res=NULL;
                free(end);
                end=NULL;
                close(multicast_sock);
                exit(1);
            }
            inc+=r;    
        }
        char * id =malloc(8);
        char *point=malloc(4);
        strncpy(id,end+1,8); 
        strncpy(point,end+1+9,4); 

        //close connection ??????
        close(multicast_sock);
        free(point);point=NULL;
        free(id);id=NULL;
        free(end);end=NULL;

    }
    free(res);
    res=NULL;
    
    
}