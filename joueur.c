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

//aux functions 
void PrependZeros(char *dest, const char *src, unsigned minimal_width);
void PrependZeros(char *dest, const char *src, unsigned minimal_width) {
  size_t len = strlen(src);
  size_t zeros = (len > minimal_width) ? 0 : minimal_width - len;
  memset(dest, '0', zeros);
  strcpy(dest + zeros, src);
}

char* remove_hashtags(char* src);
char * remove_hashtags(char*src)
{   
    int cpt=0;
    for (int i = 0; i < 15; i++)
    {
        if (src[i]=='#')
        {
            cpt++;
        }
        
    }
    int ip_len=strlen(src)-cpt;
    char * d=malloc(ip_len);
    strncpy(d,src,ip_len);
    return d;

    

}


//add the possiilite of returnning the received values 
void connection_reply(int connection_socket);
void regstr_reply(int connection_socket);
void reg_new_partie(int connection_socket,char *id ,char* udp_port);
void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game);
void unreg(int connection_socket);
void lab_size(int connection_socket,int partie);
void palyers_list(int connection_socket,int partie);
void partie_noncom_list(int connection_socket);
//debut de paritie -------------------------------
void start(int connection_socket);
void start_reply(int connection_socket);
//deroulemnt de partie ----------------------------
void move_up(int connection_socket,int dist);
void move_down(int connection_socket,int dist);
void move_left(int connection_socket,int dist);
void move_right(int connection_socket,int dist);
void move_reply(int connection_socket);
void quit(int connection_socket);
void get_list_req(int connection_socket);
void get_list_res(int connection_socket);

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
        reg_partie_existant(connection_socket,id ,udp_port,i);
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
                free(buff);
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
                        free(buff);
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
            free(buff);
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
                free(buff);
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
                    free(buff);
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
        free(buff);
        exit(1);
    }

    free(buff);

}

///////////////////////////////////////////////////////////////////////

void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game)
{
    void * buff=malloc(24);

    sprintf((char *)buff,"REGIS %s %s ",id,udp_port);
    *((uint8_t *)(buff+20))=(uint8_t)game;
    sprintf((char *)(buff+21),"%s","***");
    if (send(connection_socket,buff,24,0)<24)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        free(buff);
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
            free(buff);
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
                free(buff);
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
                free(buff);
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

    void * buff=malloc(11);
    sprintf((char*)buff,"%s","SIZE? ");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    sprintf((char *)(buff+7),"%s","***");
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        free(buff);
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
            free(buff);
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
                free(buff);
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
                    free(buff);
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
        free(buff);
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
            free(buff);
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
                free(buff);
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
                    free(buff);
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
                free(buff);
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
    if (send(connection_socket,"GAME?***",8,0)<8)
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
            free(game_reply);
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
                free(ogame);
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

//debut de partie ------------------------------------------------------------------------------------------------------------------

void start(int connection_socket)
{
    if (send(connection_socket,"START***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }


}

void start_reply(int connection_socket)
{
    int size=5+1+2+2+1+15+4+3+6;
    void * reply=malloc(size);
    int inc=0;
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
    uint16_t h=*((uint16_t *)(reply+6+2));
    uint16_t w=*((uint16_t *)(reply+6+2+3));
    uint8_t n_fantom=*((uint8_t *)(reply+6+2+3+3));
    char *ip=malloc(15);
    strncpy(ip,(char *)(reply+6+2+3+3+2),15);
    char *port=malloc(4);
    strncpy(ip,(char *)(reply+6+2+3+3+2+16),4);
    
    char *real_ip=remove_hashtags(ip);

    free(ip);
    ip=NULL;
    free(port);
    port=NULL;

    free(reply);
    reply=NULL;

    int size_pos=5+8+3+3+3+3;
    void *reply_pos=malloc(size_pos);
    inc =0;
    while (inc<size)
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
    char *id =malloc(8);
    strncpy(id,(char *)(reply_pos+6),8);
    char *x=malloc(3);
    strncpy(x,(char *)(reply_pos+6+9),3);
    char *y=malloc(3);
    strncpy(y,(char *)(reply_pos+6+9+4),3);
    
    free(reply_pos);
    reply_pos=NULL;
    
}

//deroulement de partie --------------------------------------------------------------

void move_up(int connection_socket,int dist)
{   
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","UPMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    free(req);
    req=NULL;

}

//////////////////////////////////////

void move_down(int connection_socket,int dist)
{   
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","DOMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    free(d);
    d=NULL;
    free(buff);
    buff=NULL;
    free(req);
    req=NULL;

}

////////////////////////////////////////

void move_left(int connection_socket,int dist)
{   
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","LEMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    free(d);
    d=NULL;
    free(buff);
    buff=NULL;
    free(req);
    req=NULL;

}

//////////////////////////////////

void move_right(int connection_socket,int dist)
{   
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","RIMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }

    free(d);
    d=NULL;
    free(buff);
    buff=NULL;
    free(req);
    req=NULL;


}

//////////////////////////////////////

void reply_move(int connection_socket)
{
    char * reply_msg=malloc(5);
    int inc =0;
    while (inc<5)
    {
        int r=recv(connection_socket,reply_msg+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if(strcmp("MOVE!",(char*)reply_msg)==0)
    {
        void *reply_mv=malloc(11);
        inc =0;
        while (inc<11)
        {
            int r=recv(connection_socket,reply_mv+inc,11-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        char* new_x=malloc(3);
        char* new_y=malloc(3);
        strncpy(new_x,(char*)(reply_mv+1),3);
        strncpy(new_y,(char*)(reply_mv+1+4),3);

        free(new_x);new_x=NULL;
        free(new_y);new_y=NULL;
        free(reply_mv);
        reply_mv=NULL;

    }
    else
    {
        if(strcmp("MOVEF",(char*)reply_msg)==0)
        {
            void *reply_mvf=malloc(15);
            inc =0;
            while (inc<15)
            {
                int r=recv(connection_socket,reply_mvf+inc,15-inc,0);
                if (r==-1)
                {   
                    perror("prolem while receiving\n");
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }
            char* new_x=malloc(3);
            char* new_y=malloc(3);
            char* port=malloc(4);
            strncpy(new_x,(char*)(reply_mvf+1),3);
            strncpy(new_y,(char*)(reply_mvf+1+4),3);
            strncpy(port,(char*)(reply_mvf+1+4+4),4);

            free(new_x);new_x=NULL;
            free(new_y);new_y=NULL;
            free(port);port=NULL;
            free(reply_mvf);
            reply_mvf=NULL;



        }
    }

    free(reply_msg);
    reply_msg=NULL;

}

///////////////////////////////////
void quit(int connection_socket)
{
    if (send(connection_socket,"IQUIT***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    char * reply=malloc(8);
    int inc =0;
    while (inc<8)
    {
        int r=recv(connection_socket,reply+inc,8-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    free(reply);
    reply=NULL;
}


/////////////////////////////////////////////////

void get_list(int connection_socket)
{
    if (send(connection_socket,"GLIS?***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    

}
void get_list_res(int connection_socket)
{
    void* reply=malloc(10);
    int inc =0;
    while (inc<10)
    {
        int r=recv(connection_socket,reply+inc,10-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    uint8_t nb_player=*(uint8_t *)(reply+6);
    for (int i = 0; i < nb_player; i++)
    {
        int size=6+9+4+4+4+3;
        void * player_reply=malloc(size);
        inc =0;
        while (inc<size)
        {
            int r=recv(connection_socket,player_reply+inc,size-inc,0);
            if (r==-1)
            {      
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        char * id=malloc(8);
        strncpy(id,(char*)(player_reply+6),8);
        char * x=malloc(3);
        strncpy(x,(char*)(player_reply+6+9),3);
        char * y=malloc(3);
        strncpy(y,(char*)(player_reply+6+9+4),3);
        char * point=malloc(4);
        strncpy(point,(char*)(player_reply+6+9+4+4),4);

        free(id);
        id=NULL;
        free(x);
        x=NULL;
        free(y);
        y=NULL;
        free(point);
        point=NULL;
        free(player_reply);
        player_reply=NULL;
    }
    

    free(reply);
    reply=NULL;

}

void muilti_def_mail(char * ip_multicast)
{

}