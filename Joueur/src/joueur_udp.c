#include "joueur_udp.h"

//recevoir un message udp
void * recv_udp_message(void * arg){
    int port=atoi((char *)arg);
    int sock=socket(PF_INET,SOCK_DGRAM,0);
    struct sockaddr_in adress_sock;
    adress_sock.sin_family=AF_INET;
    adress_sock.sin_port=htons(port);
    int r=bind(sock,(struct sockaddr *)&adress_sock,sizeof(struct sockaddr_in));
    if(r!=0){
        perror("erreur lors du bind\n");
        exit(1);
    }
    char buff[219];
    while (1)
    {
        int rec=recv(sock,buff,218,0);
        buff[rec-3]='\0';
        buff[14]='\0';
        printf("received from : %s \n\t message: %s\n",(char*)buff+6,(char*)buff+15);

    }
}