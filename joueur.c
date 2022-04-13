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
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <assert.h>
#include <netdb.h> 

void connection_reply(int connection_socket);
void regstr_reply(int connection_socket);
void reg_new_partie(int connection_socket,char *id ,char* udp_port);
void reg_partie_existant(int connection_socket,int game,char *id ,char* udp_port);
void unreg(int connection_socket);
void lab_size(int connection_socket,int partie);
void palyers_list(int connection_socket,int partie);
void partie_noncom_list(int connection_socket);
void start(int connection_socket);

int main(int argc, char const *argv[]){
    assert(argc==2);
    //generate identifiant sur 8 character soit par scanf soit par random
    char *id=malloc(8); //must be with malloc to avoid problem when passong them as paramaters 
    char* udp_port=malloc(4);
    id="fhdjp125" ;
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
        int i;
        connection_reply(connection_socket);
        scanf("%d",&i);
        printf("pk\n");
        reg_partie_existant(connection_socket,i,id ,udp_port);
        regstr_reply(connection_socket);
        unreg(connection_socket);
        scanf("%d",&i);
        palyers_list(connection_socket,i);
        partie_noncom_list(connection_socket);
        //partie_noncom_list(connection_socket);

        /*selection menu for 2 possibiliies actually it's gonna be more it's gonna containe all the optionam req */
        /*int select;
        printf("select: \n1 to start new game or \n2 to join an already running game \n3 for more\n");
        scanf("%d",&select);
        void * registre_partie=malloc(24);
        switch (select)
        {
            case 1: //newpl
                
            break;
            
            case 2://regis
               
            break;

            case 3://more all of this should be default whether registred or not  
                printf("select : \n 1 desinscrire \n2 demander la taille de labyrinthe \n3 demander la list des joueurs \n4 demander listde partie qui ne sont pas encore commencee \n5 to start");
                int select2;
                scanf("%d",&select2);
                switch (select2)
                {
                    case 1://desinscrire
                        

                    break;

                    case 2://size lab
                        
                    break;

                    case 3://list player
                        
                    
                    break;

                    case 4: //partie non commencéé
                        
                    break;

                    case 5://empty 
                    break;
                }
            break;
        }

        //reply reg
       
        //start
        */



        
        

    }

    
    return 0;
}

//////////////////////////////////////////////////////////////////

void connection_reply(int connection_socket)
{
        void * buff=malloc(13);
        int inc=0;
        while (inc<10)
        {
            int r=recv(connection_socket,buff+inc,10-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint8_t nbr_partie=(*((uint8_t *)(buff+6))) ;
        *(char *)(buff+inc)='\0';
        printf("%s ttt %d\n",(char*)(buff),nbr_partie);

        for (uint8_t i = 0; i <nbr_partie; i++)
        {
            inc=0;
            while (inc<12)
            {
                int r=recv(connection_socket,buff+inc,12-inc,0);
                if (r==-1)
                    {
                        perror("prolem while receiving\n");
                        close(connection_socket);
                        exit(1);
                    }
                inc+=r;    
            }
            uint8_t num_partie=*((uint8_t *)(buff+6));
            uint8_t num_joueur_inscr=*((uint8_t *)(buff+8));
            //display results for the user
            printf("%d- game %d with %d player\n",i,num_partie,num_joueur_inscr);

        }

        free(buff);

}

//////////////////////////////////////////////////////////////////

void regstr_reply(int connection_socket)
{   
    char *buff=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {
            perror("pbrolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }


    if(strncmp(buff,"REGNO",5)==0)
    {
        
        inc=0;
        while (inc<3)
        {
            int r=recv(connection_socket,buff+inc,3-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        printf("refused\n");

        
    }
    else
    {
        if (strncmp(buff,"REGOK",5)==0)
        {
            inc=0;
            while (inc<5)
            {
                int r=recv(connection_socket,buff+inc,5-inc,0);
                if (r==-1)
                {
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }

            printf("accepted in partie num %d\n",*((uint8_t *)(buff+1)));
        }
        
    }

    free(buff);

}

/////////////////////////////////////////////////////////////////

void reg_new_partie(int connection_socket,char *id ,char* udp_port)
{
    //id and udp port must be created using malloc so we can avoid them overflowing
    char * buff=malloc(22);
    sprintf(buff,"NEWPL %s %s***",id,udp_port);
    
    if (send(connection_socket,buff,22,0)<22)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    free(buff);

}

///////////////////////////////////////////////////////////////////////

void reg_partie_existant(int connection_socket,int game,char *id ,char* udp_port)
{
    void * buff=malloc(24);

    sprintf((char *)buff,"REGIS %s %s ",id,udp_port);
    *((uint8_t *)(buff+20))=(uint8_t)game;
    sprintf((char *)(buff+21),"%s","***");
    if (send(connection_socket,buff,24,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    free(buff);
}

//////////////////////////////////////////

void unreg(int connection_socket)
{
    if (send(connection_socket,"UNREG***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    char * buff=malloc(5);
    int inc=0;

    while (inc<5)
    {
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    if(strncmp(buff,"UNROK",5)==0)
    {
        inc=0;
        while (inc<5)
        {
            int r=recv(connection_socket,buff+inc,5-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        printf("desinscription de partie num %d\n",*((uint8_t *)(buff+1)));
    }
    else if(strncmp(buff,"DUNNO",5)==0)
    {
        printf("vous n'etes pas encore inscrit\n");
        inc=0;
        while (inc<3)
        {
            int r=recv(connection_socket,buff+inc,3-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

    }

    free(buff);

}

///////////////////////////////////////////

void lab_size(int connection_socket,int partie)
{
    /*printf("uuuuuuuuuuuuuuuuuuuuu lab size\n");
    printf("quelle partie ?\n");
    int partie;
    scanf("%d",&partie);*/

    void * buff=malloc(11);
    sprintf((char*)buff,"%s","SIZE? ");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    sprintf((char *)(buff+7),"%s","***");
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    if(strncmp(buff,"SIZE!",5)==0)
    {
        inc=0;
        while (inc<11)
        {
            int r=recv(connection_socket,buff+inc,11-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        uint16_t hauteur=ntohs(*((uint16_t *)(buff+1+1+1))); //little endian
        uint16_t largeur=ntohs(*((uint16_t *)(buff+3+2+1))); //little endian
        printf("hauteur:  %d     largeur :%d \n",hauteur,largeur);
    }
    else
    {
        if(strncmp(buff,"DUNNO",5)==0)
        {
            inc=0;
            while (inc<3)
            {
                int r=recv(connection_socket,buff+inc,3-inc,0);
                if (r==-1)
                {
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }

            printf("partie non trouvé \n");
        }
    }
    
    free(buff);

}

////////////////////////////////////////

void palyers_list(int connection_socket,int partie)
{
    void * buff=malloc(10);
    sprintf((char*)buff,"%s","LIST? ");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    sprintf((char *)(buff+6+1),"%s","***");
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    if(strncmp(buff,"LIST!",5)==0)
    {
        inc=0;
        while (inc<7)
        {
            int r=recv(connection_socket,buff+inc,7-inc,0);
            if (r==-1)
            {
                perror("problem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        uint8_t nb_player= *((uint8_t *)(buff+3));
        printf("players: \n");
        char * player=malloc(18);
        char* player_id=malloc(9);
        for (uint8_t i = 0; i < nb_player; i++){
            inc=0;
            while (inc<17)
            {
                int r=recv(connection_socket,player+inc,17-inc,0);
                if (r==-1)
                {   
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }
            strncpy(player_id,player+6,8); 
            player_id[8]='\0';
            printf("\t player: %s\n",player_id);                   
        }
        free(player_id);
        free(player);                 
    }
    else if(strncmp(buff,"DUNNO",5)==0){
        
        inc=0;
        while (inc<3)
        {
            int r=recv(connection_socket,buff+inc,3-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        printf("partie non trouvé \n");
        
    }

    free(buff);

}

////////////////////////////////////////////////

void partie_noncom_list(int connection_socket)
{
    if (send(connection_socket,"GAME?***",8,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    void * game_reply=malloc(10);
    int inc=0;
    while (inc<10)
    {
        int r=recv(connection_socket,game_reply+inc,10-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    uint8_t nb_games=*((uint8_t *)(game_reply+6));
    printf("list partie non commencer %d:\n",nb_games);
    void * ogame=malloc(12);
    for (uint8_t i = 0; i < nb_games; i++)
    {
        inc=0;
        while (inc<12)
        {
            int r=recv(connection_socket,ogame+inc,12-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
                            
        int num_partie=*((uint8_t *)(ogame+6));
        int nb_player_per_partie=*((uint8_t *)(ogame+8));

        printf("\t partie num : %d    avec %d joueurs\n",num_partie,nb_player_per_partie);
                            
    }

    free(game_reply);
    free(ogame);

}

///////////////////////////////////////////

void start(int connection_socket)
{
    if (send(connection_socket,"START***",8,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }


}
