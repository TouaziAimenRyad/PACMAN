/*
!!!!!!!!!!!!!!!! ALL NUMBERS RECEIVIED TRANSFORM TO LITTLE ENDIAN
!!!!!!!!!!!!!!!! check list players there's possiblly a problem 
make a user menu for input cause you'll need it 
make the optional reqs
check for what to do in case of being refused to participate in the game 
!!!!!! to make it easier for user interaction rendre le proframme plus procedural chaque case function call would be much better and you can't declare variables inside a swicth statment
*/

/*
recevoire [GAME n***] nsur 1 octet
recevoire n fois [OGAME m s***] m s sur 1 octet
2 possibilité 
    -envoyer [NEWPL id port***] id identifiant jr sur 8 caractere et port udp sur 4 caractere 
    -envoyer [REGIS id port m***] m numero de partie a choisir de mssg recoit du server
2possibilité 
    -recevoire [REGOK m***] m est le numero de partie soit un num deja existant soit nouveau
    -recevoire [REGNO***] si l'inscription n'est pas accepter
envoyer [START***] pour commencer la partie 
il ya des requetes a envoyer avant que la partie commence 
*/

#include "joueur.h"
//aux functions 


//add the possiilite of returnning the received values 

//debut de paritie -------------------------------

//deroulemnt de partie ----------------------------



int main(int argc, char const *argv[]){
    assert(argc==2);
    //generate identifiant sur 8 character soit par scanf soit par random
    char *id=malloc(8); //must be with malloc to avoid problem when passong them as paramaters 
    char* udp_port=malloc(4);
    id="fhdjp128" ;
    udp_port="8000";
    /*char player_info[12];

    strcat(player_info,id);
    strcat(player_info," ");
    strcat(player_info,udp_port);*/


    struct sockaddr_in server_adr;
    /*
     //recuperer l'adresse du lulu
    struct hostent *server;
    server=gethostbyname("lulu.informatique.univ-paris-diderot.fr"); 
    if(server==NULL)
    {
        perror("couldn't find the host\n");
        exit(1);
    }*/

    /*struct in_addr **addresses=(struct in_addr**)server->h_addr_list;*/
    server_adr.sin_family=AF_INET;
    server_adr.sin_port=htons((uint16_t)atoi(argv[1]));
    inet_aton("127.0.0.1",&server_adr.sin_addr);  /*(**addresses);*/

    int size=sizeof(server_adr);

    int connection_socket=socket(PF_INET,SOCK_STREAM,0);

    if(connect(connection_socket,(struct sockaddr *)&server_adr,(socklen_t)size)==0)
    {
        int choice;
        int choice2;
        int partie;
        int t=0;
        int r=0;
       //connection_reply(connection_socket);

        while(t==0)
        {
            printf("input 1 for new new game\ninput 2 to join an existing game\ninput 3 to unregistre from a game\ninput 4 for lab size\ninput 5 for player list\ninput 6 for game list\n");
            scanf("%d",&choice);//tried with read 0 caused some weird problems 
            switch (choice)
            {
                case 1:
                    t=1;
                    reg_new_partie(connection_socket,id,udp_port);
                    regstr_reply(connection_socket);
                    while (r==0)
                    {
                        printf("input 1 to unregistre from a game\ninput 2 for lab size\ninput 3 for player list\ninput 4 for game list\ninput 5 to start\n");
                        scanf("%d",&choice2);
                        switch (choice2)
                        {
                        case 1:
                            unreg(connection_socket);
                        break;

                        case 2:
                            printf("what game\n");
                            scanf("%d",&partie);
                            lab_size(connection_socket,partie);
                        break;

                        case 3:
                            printf("what game\n");
                            scanf("%d",&partie);
                            palyers_list(connection_socket,partie);
                        break;

                        case 4:
                            partie_noncom_list(connection_socket);
                        break;

                        case 5:
                            r=1;
                            start(connection_socket);
                            start_reply(connection_socket);
                        break;
                        
                        }
                    }
                break;

                case 2:
                    t=1;
                    printf("what game you wanna join\n");
                    scanf("%d",&partie);
                    reg_partie_existant(connection_socket,id,udp_port,partie);
                    regstr_reply(connection_socket);
                    while (r==0)
                    {
                        printf("input 1 to unregistre from a game\ninput 2 for lab size\ninput 3 for player list\ninput 4 for game list\ninput 5 to start\n");
                        scanf("%d",&choice2);
                        switch (choice2)
                        {
                        case 1:
                            unreg(connection_socket);
                        break;

                        case 2:
                            printf("what game\n");
                            scanf("%d",&partie);
                            lab_size(connection_socket,partie);
                        break;

                        case 3:
                            printf("what game\n");
                            scanf("%d",&partie);
                            palyers_list(connection_socket,partie);
                        break;

                        case 4:
                            partie_noncom_list(connection_socket);
                        break;

                        case 5:
                            r=1;
                            start(connection_socket);
                            start_reply(connection_socket);
                        break;
                        
                        }
                    }
                break;

                case 3:
                    unreg(connection_socket);

                break;

                case 4:
                    printf("what game\n");
                    scanf("%d",&partie);
                    lab_size(connection_socket,partie);
                break;

                case 5:
                    printf("what game\n");
                    scanf("%d",&partie);
                    palyers_list(connection_socket,partie);
                break;

                case 6:
                    partie_noncom_list(connection_socket);
                break;
        
            }
        }

        printf("out\n");

        int choice3;
        int distance;
        int d=0;
        while (d==0)
        {
            printf("input 1 to move up\ninput 2 to move down\ninput 3 to move left\ninput 4 to move right\ninput 5 to get player list\n input 6 to send mail to all \ninput 7 to send private msg\n input 8 to quit\n");
            scanf("%d",&choice3);
            switch (choice3)
            {
                case 1:
                    printf("input distance\n");
                    scanf("%d",&distance);
                    move_up(connection_socket,distance);
                    reply_move(connection_socket);
                break;

                case 2:
                    printf("input distance\n");
                    scanf("%d",&distance);
                    move_down(connection_socket,distance);
                    reply_move(connection_socket);
                
                break;

                case 3:
                    printf("input distance\n");
                    scanf("%d",&distance);
                    move_left(connection_socket,distance);
                    reply_move(connection_socket);
                break;

                case 4:
                    printf("input distance\n");
                    scanf("%d",&distance);
                    move_right(connection_socket,distance);
                    reply_move(connection_socket);
                break;

                case 5:
                    get_list_req(connection_socket);
                    get_list_res(connection_socket);
                break;

                case 6:
                    printf("input message \n");
                    char buff[200];
                    read(0,buff,200);
                    printf("%s\n",buff);
                    send_muilti_def_mail(connection_socket,buff);
                    
                break;

                case 7:
                    printf("send private udp msg\n");
                break;

                case 8:
                    quit(connection_socket);
                break;
            
            
            }
        }
        
        
        
        
        

    }

    
    return 0;
}

//////////////////////////////////////////////////////////////////


///////////////////////////////////////////

//debut de partie ------------------------------------------------------------------------------------------------------------------


//deroulement de partie --------------------------------------------------------------



///////////////////////////////////////////////////////////////
