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
void reg_partie_existant(int connection_socket,char *id ,char* udp_port);
void unreg(int connection_socket);
void lab_size(int connection_socket);
void palyers_list(int connection_socket);
void partie_noncom_list(int connection_socket);
void start(int connection_socket);

int main(int argc, char const *argv[])
{
    assert(argc==2);
    //generate identifiant sur 8 character soit par scanf soit par random
    char *id=malloc(8); //must be with malloc to avoid problem when passong them as paramaters 
    char* udp_port=malloc(4);
    id="fhdjs125" ;
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
    server_adr.sin_addr.s_addr=htonl(INADDR_ANY);  /*(**addresses);*/

    int size=sizeof(server_adr);

    int connection_socket=socket(PF_INET,SOCK_STREAM,0);

    if(connect(connection_socket,(struct sockaddr *)&server_adr,(socklen_t)size)==0)
    {
        connection_reply(connection_socket);
       

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
     void * game1=malloc(10);
        int inc=0;
        while (inc<10)
        {
            int r=recv(connection_socket,game1,10-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        int nbr_partie=(int)(*((uint8_t *)(game1+6))) ;
        printf("%s ttt %d\n",(char*)(game1),nbr_partie);

        for (int i = 0; i <nbr_partie; i++)
        {
            inc=0;
            void * game2=malloc(12);
            while (inc<12)
            {
                int r=recv(connection_socket,game2,12-inc,0);
                if (r==-1)
                    {
                        perror("prolem while receiving\n");
                        close(connection_socket);
                        exit(1);
                    }
                inc+=r;    
            }
            uint8_t num_partie=*((uint8_t *)(game2+6));
            uint8_t num_joueur_inscr=*((uint8_t *)(game2+6+sizeof(uint8_t)+1));
            //display results for the user
            printf("%d- game %d with %d player\n",i,num_partie,num_joueur_inscr);
            printf("%d\n",i);

        }

}

//////////////////////////////////////////////////////////////////

void regstr_reply(int connection_socket)
{   
    void *registre_reply1=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,registre_reply1,5-inc,0);
        if (r==-1)
        {
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    char registre_reply2[5];
    strcpy(registre_reply2,(char *)registre_reply1);

    if(strcmp(registre_reply2,"REGNO")==0)
    {
        
        void* buff=malloc(3);
        inc=0;
        while (inc<3)
        {
            int r=recv(connection_socket,buff,3-inc,0);
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
        if (strcmp(registre_reply2,"REGOK")==0)
        {
            void* registre_reply3=malloc(5);
            inc=0;
            while (inc<5)
            {
                int r=recv(connection_socket,registre_reply3,5-inc,0);
                if (r==-1)
                {
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }

            printf("accepted in partie num %d\n",*((uint8_t *)(registre_reply3+1)));
        }
        
    }

}

/////////////////////////////////////////////////////////////////

void reg_new_partie(int connection_socket,char *id ,char* udp_port)
{
    //id and udp port must be created using malloc so we can avoid them overflowing
    void * registre_partie=malloc(22);
    sprintf((char*)registre_partie,"NEWPL %s %s***",id,udp_port);
    
    if (send(connection_socket,registre_partie,22,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

}

///////////////////////////////////////////////////////////////////////

void reg_partie_existant(int connection_socket,char *id ,char* udp_port)
{
    printf("zzzzzzzzzzzzzzzzzzzz reg partie exist\n");
    void * registre_partie=malloc(24);
    printf("entrer la partie voulu:\n");
    int partie_volu;//uint8_t
    scanf("%d",&partie_volu);
    sprintf((char *)registre_partie,"%s %s %s ","REGIS",id,udp_port);
    *((uint8_t *)(registre_partie+6+8+1+4+1))=partie_volu;
    sprintf((char *)(registre_partie+6+8+1+4+1+1),"%s","***");
    if (send(connection_socket,registre_partie,24,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
}

//////////////////////////////////////////

void unreg(int connection_socket)
{
    if (send(connection_socket,"UNREG***",8,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    void * unreg_reply=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,unreg_reply,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    char unreg_reply_msg[5];
    strcpy(unreg_reply_msg,(char*)unreg_reply);
    if(strcmp(unreg_reply_msg,"UNROK")==0)
    {
        void* buff=malloc(5);
        inc=0;
        while (inc<5)
        {
            int r=recv(connection_socket,buff,5-inc,0);
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
    else
    {
        if(strcmp(unreg_reply_msg,"DUNNO")==0)
        {
            printf("vous n'etes pas encore inscrit\n");
            void* buff=malloc(3);
            inc=0;
            while (inc<3)
            {
                int r=recv(connection_socket,buff,3-inc,0);
                if (r==-1)
                {
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }

        }
    }

}

///////////////////////////////////////////

void lab_size(int connection_socket)
{
    printf("uuuuuuuuuuuuuuuuuuuuu lab size\n");
    printf("quelle partie ?\n");
    int partie;
    scanf("%d",&partie);
    void * request_size=malloc(10);
    sprintf((char*)request_size,"%s","SIZE? ");
    *((uint8_t*)(request_size+6))=(uint8_t)partie;
    sprintf((char *)(request_size+6+1),"%s","***");
    if (send(connection_socket,request_size,10,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    void * size_reply=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,size_reply,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    char size_reply_msg[5];
    strcpy(size_reply_msg,(char*)size_reply);
    if(strcmp(size_reply_msg,"SIZE!")==0)
    {
        void* buff=malloc(11);
        inc=0;
        while (inc<11)
        {
            int r=recv(connection_socket,buff,11-inc,0);
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
        if(strcmp(size_reply_msg,"DUNNO")==0)
        {
            void* buff=malloc(3);
            inc=0;
            while (inc<3)
            {
                int r=recv(connection_socket,buff,3-inc,0);
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
    
}

////////////////////////////////////////

void palyers_list(int connection_socket)
{
    printf("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz list player\n");
    printf("quelle partie ?\n");
    int partie;
    scanf("%d",&partie);
    void * request_list=malloc(10);
    sprintf((char*)request_list,"%s","LIST? ");
    *((uint8_t*)(request_list+6))=(uint8_t)partie;
    sprintf((char *)(request_list+6+1),"%s","***");
    if (send(connection_socket,request_list,10,0)<0)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    void * list_reply=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,list_reply,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    char list_reply_msg[5];
    strcpy(list_reply_msg,(char*)list_reply);
    if(strcmp(list_reply_msg,"LIST!")==0)
    {
        void* buff=malloc(7);
        inc=0;
        while (inc<7)
        {
            int r=recv(connection_socket,buff,7-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        int nb_player= *((uint8_t *)(buff+1+1+1));
        printf("players: \n");
        for (int i = 0; i < nb_player; i++)
        {
            void * player=malloc(17);
            inc=0;
            while (inc<17)
            {
                int r=recv(connection_socket,player,17-inc,0);
                if (r==-1)
                {   
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }
            char* player_id=malloc(8);
            strcpy(player_id,(char*)(player+5+1)); //!!!!this might have a problem it takes the stars with 
            printf("\t player: %s\n",(char*)player);
            free(player);
            player=NULL;
            free(player_id);
            player_id=NULL;
                                
        }
                           
    }
    else
    {
        if(strcmp(list_reply_msg,"DUNNO")==0)
        {
            void* buff=malloc(3);
            inc=0;
            while (inc<3)
            {
                int r=recv(connection_socket,buff,3-inc,0);
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
        int r=recv(connection_socket,game_reply,10-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    int nb_games= *((uint8_t *)(game_reply+5+1));
    printf("list partie non commencer:\n");
    for (int i = 0; i < nb_games; i++)
    {
        void * ogame=malloc(12);
        inc=0;
        while (inc<12)
        {
            int r=recv(connection_socket,ogame,12-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
                            
        int num_partie=*((uint8_t *)(ogame+5+1));
        int nb_player_per_partie=*((uint8_t *)(ogame+5+1+1+1));

        printf("\t partie num : %d    avec %d joueurs\n",num_partie,nb_player_per_partie);
                            
    }
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
