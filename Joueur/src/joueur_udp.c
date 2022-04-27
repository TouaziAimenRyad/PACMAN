#include "joueur_udp.h"

void * recv_udp_message(void * arg)
{
    int port=atoi((char *)arg);
    int sock=socket(PF_INET,SOCK_DGRAM,0);
    struct sockaddr_in adress_sock;
    adress_sock.sin_family=AF_INET;
    adress_sock.sin_port=htons(port);
    int r=bind(sock,(struct sockaddr *)&adress_sock,sizeof(struct sockaddr_in));
    char buff[218];
    char id[8];
    while (1)
    {
        int rec=recv(sock,buff,218,0);
        buff[rec]='\0';
        strncpy(id,buff+6,8);
        printf("received from : %s \n\t message: %s",id,buff+15);

    }
    

}