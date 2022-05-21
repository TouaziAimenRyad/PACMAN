#include "joueur.h"


int main(int argc, char const *argv[]){
    assert(argc==2);
    char id[9];
    char udp_port[5];
    char udp2[5];
    char id2[9];
    int ret;
    //recuperer le id et l'udp port
    memset(id,'0',8);
    printf("donner votre id\n");
    lire_chaine(id,8);
    do{
        printf("donner le port\n");
        memset(udp_port,0,4);
        lire_chaine(udp_port,4);
    }while(1023>=atoi(udp_port) || atoi(udp_port)>9999);
    id[8]='\0';
    udp_port[4]='\0';
    struct sockaddr_in server_adr;
    //recuperer l'@ de lulu
    struct addrinfo *first_info;
    struct addrinfo hints;
    memset(&hints,0,sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype =SOCK_STREAM;
    int r=getaddrinfo("lulu.informatique.univ-paris-diderot.fr",NULL,&hints,&first_info);
    if(r!=0){
        perror("erreur lors du getaddrinfo");
        exit(1);
    }
    struct addrinfo *info=first_info;
    if(info!=NULL){
        struct sockaddr *saddr=info->ai_addr;
        struct sockaddr_in *addressin=(struct sockaddr_in *)saddr;
        server_adr.sin_addr=(struct in_addr) (addressin->sin_addr);
    }
    else{
        perror("erreur aucune adresse ip pour lulu");
        exit(1);
    }
    freeaddrinfo(first_info);
    server_adr.sin_family=AF_INET;
    server_adr.sin_port=htons((uint16_t)atoi(argv[1]));
    int size=sizeof(server_adr);

    int connection_socket=socket(PF_INET,SOCK_STREAM,0);
    //connecter avec le serveur
    if(connect(connection_socket,(struct sockaddr *)&server_adr,(socklen_t)size)==0)
    {
        int choice;
        int partie,hauteur,largeur;
        int c=0;
        int type=0;
        connection_reply(connection_socket);
        
        printf("input 1 for new game\ninput 2 to join an existing game\ninput 3 to unregistre from a game\ninput 4 for lab size\ninput 5 for player list\ninput 6 for non started games list\ninput 7 for started game list\ninput 8 for start game\ninput 9 for players list in started games\ninput 10 to change id\ninput 11 to change udp port messages\ninput 12 new special game\n");

        while(c==0){
            printf("\ninput your choice\n");

            scanf("%d",&choice);;
            switch (choice){
                case 1 :
                    reg_new_partie(connection_socket,id,udp_port,0,0,0);
                    regstr_reply(connection_socket);
                    break;
                case 2 :
                    printf("what game you wanna join\n");
                    do{
                        lire_entier(&partie);
                    }while(partie<=0 || partie>255);
                    reg_partie_existant(connection_socket,id,udp_port,partie);
                    regstr_reply(connection_socket);
                    break;
                case 3 :
                    unreg(connection_socket);
                    c=0;
                    break;
                case 4 :
                    printf("which game\n");
                    do{
                        lire_entier(&partie);
                    }while(partie<=0 || partie>255);
                    lab_size(connection_socket,partie);
                    break;
                case 5 :
                    printf("which game\n");
                    do{
                        lire_entier(&partie);
                    }while(partie<=0 || partie>255);
                    palyers_list(connection_socket,partie);
                    break;
                case 6 :
                    partie_noncom_list(connection_socket);
                    break;
                case 7 :
                    partie_com_list(connection_socket);
                    break;    
                case 8 :
                    start(connection_socket);
                    c=start_reply(connection_socket,udp_port);
                    break;
                case 9 :
                    printf("which game\n");
                    do{
                        lire_entier(&partie);
                    }while(partie<=0 || partie>255);
                    palyers_list_com(connection_socket,partie);
                    break;
                case 10:
                    printf("donner le nouveau id:\n");
                    memset(id2,'0',8);
                    id2[8]='\0';
                    lire_chaine(id2,8);
                    ret=change_id(connection_socket,id2);
                    if(!ret)strcpy(id,id2);
                    break;
                case 11:
                    do{
                        printf("donner le nouveau port\n");
                        memset(udp2,0,4);
                        lire_chaine(udp2,4);
                    }while(1023>=atoi(udp2) || atoi(udp2)>9999);
                    udp2[4]='\0';
                    ret=change_udp(connection_socket,udp2);
                    if(!ret)strcpy(udp_port,udp2);
                    break;
                case 12:
                    do{
                        printf("donner la hauteur\n");
                        lire_entier(&hauteur);
                    }while(hauteur<=3 || hauteur>999);
                    do{
                        printf("donner la largeur\n");
                        lire_entier(&largeur);
                    }while(largeur<=3 || largeur>999);
                    reg_new_partie(connection_socket,id,udp_port,1,(uint16_t)hauteur,(uint16_t)largeur);
                    regstr_reply(connection_socket);
                    type=1;
                    break;
                default :
                    printf("input 1 for new game\ninput 2 to join an existing game\ninput 3 to unregistre from a game\ninput 4 for lab size\ninput 5 for player list\ninput 6 for non started games list\ninput 7 for started game list\ninput 8 for start game\ninput 9 for players list in started games\ninput 10 to change id\ninput 11 to change udp port messages\ninput 12 new special game\n");
            }
        }

        int choice3;
        int distance;
        int d=0;
        char buff[201];
        //game has started
        printf("input 1 to move up\ninput 2 to move down\ninput 3 to move left\ninput 4 to move right\ninput 5 to get player list\ninput 6 to send mail to all \ninput 7 to send private msg\ninput 8 to quit\n");
        while (d==0)
        {
            printf("\ninput your choice\n");
            scanf("%d",&choice3);
            switch (choice3)
            {
                case 1:
                    printf("input distance\n");
                    do{
                        lire_entier(&distance);
                    }while(distance<0 || distance>999);
                    move_up(connection_socket,distance);
                    ret=reply_move(connection_socket);
                    if(ret)d=1;
                break;

                case 2:
                    printf("input distance\n");
                    do{
                        lire_entier(&distance);
                    }while(distance<0 || distance>999);
                    move_down(connection_socket,distance);
                    ret=reply_move(connection_socket);
                    if(ret)d=1;
                
                break;

                case 3:
                    printf("input distance\n");
                    do{
                        lire_entier(&distance);
                    }while(distance<0 || distance>999);
                    move_left(connection_socket,distance);
                    ret=reply_move(connection_socket);
                    if(ret)d=1;
                break;

                case 4:
                    printf("input distance\n");
                    do{
                        lire_entier(&distance);
                    }while(distance<0 || distance>999);
                    move_right(connection_socket,distance);
                    ret=reply_move(connection_socket);
                    if(ret)d=1;
                break;

                case 5:
                    get_list_req(connection_socket);
                    ret=get_list_res(connection_socket);
                    if(ret)d=1;
                break;

                case 6:
                    memset(buff,0,strlen(buff));
                    printf("input message \n");
                    lire_chaine(buff,200);
                    ret=send_muilti_def_mail(connection_socket,buff);
                    if(ret)d=1;
                    
                break;

                case 7:
                    memset(buff,0,strlen(buff));
                    printf("donner l'id du joueur:\n");
                    memset(id2,'0',8);
                    id2[8]='\0';
                    lire_chaine(id2,8);
                    printf("send private udp msg\n");
                    lire_chaine(buff,200);
                    ret=send_private_msg(connection_socket,id2,buff);
                    if(ret)d=1;
                break;

                case 8:
                    quit(connection_socket);
                    d=1;
                break;
                case 9:
                    if(type){
                        ret=afficher_labyrinthe(connection_socket);
                        if(ret)d=1;
                    }
                    else printf("input 1 to move up\ninput 2 to move down\ninput 3 to move left\ninput 4 to move right\ninput 5 to get player list\ninput 6 to send mail to all \ninput 7 to send private msg\ninput 8 to quit\n");
                break;
                case 10:
                    if(type){
                        ret=ghost_positions(connection_socket);
                        if(ret)d=1;
                    }
                    else printf("input 1 to move up\ninput 2 to move down\ninput 3 to move left\ninput 4 to move right\ninput 5 to get player list\ninput 6 to send mail to all \ninput 7 to send private msg\ninput 8 to quit\n");
                break;
                default:
                    printf("input 1 to move up\ninput 2 to move down\ninput 3 to move left\ninput 4 to move right\ninput 5 to get player list\ninput 6 to send mail to all \ninput 7 to send private msg\ninput 8 to quit\n");
                    break;
            }
        }
    }
    return 0;
}