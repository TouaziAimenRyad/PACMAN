
#include "joueur_avant_deb.h"
void connection_reply(int connection_socket)
{
        printf("connection reply\n");
        void * buff=malloc(13);
        int inc=0;
        while (inc<10)
        {
            int r=recv(connection_socket,buff+inc,10-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                free(buff);
                buff=NULL;
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
                        free(buff);
                        buff=NULL;
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
        buff=NULL;

}

//////////////////////////////////////////////////////////////////

void regstr_reply(int connection_socket)
{   
    printf("registre reply\n");
    char *buff=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {
            perror("pbrolem while receiving\n");
            free(buff);
            buff=NULL;
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
                free(buff);
                buff=NULL;
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
    buff=NULL;

}

/////////////////////////////////////////////////////////////////

void reg_new_partie(int connection_socket,char *id ,char* udp_port)
{
    printf("registre new partie\n");
    //id and udp port must be created using malloc so we can avoid them overflowing
    char * buff=malloc(22);
    sprintf(buff,"NEWPL %s %s***",id,udp_port);
    
    if (send(connection_socket,buff,22,0)<22)
    {
        perror("prolem while sending\n");
        free(buff);
        buff=NULL;
        close(connection_socket);
        exit(1);
    }

    free(buff);
    buff=NULL;

}

///////////////////////////////////////////////////////////////////////

void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game)
{
    printf("registre partie existant\n");
    void * buff=malloc(24);

    sprintf((char *)buff,"REGIS %s %s ",id,udp_port);
    *((uint8_t *)(buff+20))=(uint8_t)game;
    sprintf((char *)(buff+21),"%s","***");
    if (send(connection_socket,buff,24,0)<24)
    {
        perror("prolem while sending\n");
        free(buff);
        buff=NULL;
        close(connection_socket);
        exit(1);
    }

    free(buff);
    buff=NULL;
}

//////////////////////////////////////////

void unreg(int connection_socket)
{
    printf("unregistre\n");
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
            free(buff);
            buff=NULL;
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
                free(buff);
                buff=NULL;
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
                free(buff);
                buff=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

    }

    free(buff);
    buff=NULL;

}

///////////////////////////////////////////

void lab_size(int connection_socket,int partie)
{
    printf("lab size\n");
    void * buff=malloc(11);
    sprintf((char*)buff,"%s","SIZE? ");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    sprintf((char *)(buff+7),"%s","***");
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        free(buff);
        buff=NULL;
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
            free(buff);
            buff=NULL;
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
                free(buff);
                buff=NULL;
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
                    free(buff);
                    buff=NULL;
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }

            printf("partie non trouvé \n");
        }
    }
    
    free(buff);
    buff=NULL;

}

////////////////////////////////////////

void palyers_list(int connection_socket,int partie)
{
    printf("player list before playng\n");
    void * buff=malloc(10);
    sprintf((char*)buff,"%s","LIST? ");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    sprintf((char *)(buff+6+1),"%s","***");
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        free(buff);
        buff=NULL;
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
            free(buff);
            buff=NULL;
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
                free(buff);
                buff=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        uint8_t nb_player= *((uint8_t *)(buff+3));
        printf("players: \n");
        
        for (uint8_t i = 0; i < nb_player; i++){
            char * player=malloc(18);
            char* player_id=malloc(9);
            inc=0;
            while (inc<17)
            {
                int r=recv(connection_socket,player+inc,17-inc,0);
                if (r==-1)
                {   
                    perror("prolem while receiving\n");
                    free(buff);
                    buff=NULL;
                    free(player_id);player_id=NULL;
                    free(player);player=NULL;
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }
            strncpy(player_id,player+6,8); 
            player_id[8]='\0';
            printf("\t player: %s\n",player_id); 
            free(player_id);player_id=NULL;
            free(player);player=NULL;                  
        }
                      
    }
    else if(strncmp(buff,"DUNNO",5)==0){
        
        inc=0;
        while (inc<3)
        {
            int r=recv(connection_socket,buff+inc,3-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                free(buff);
                buff=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        printf("partie non trouvé \n");
        
    }

    free(buff);
    buff=NULL;

}

////////////////////////////////////////////////

void partie_noncom_list(int connection_socket)
{
    printf("partie noncommencer\n");
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
            free(game_reply);
            game_reply=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    uint8_t nb_games=*((uint8_t *)(game_reply+6));
    printf("list partie non commencer %d:\n",nb_games);
   
    for (uint8_t i = 0; i < nb_games; i++)
    {
        void * ogame=malloc(12);
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
        free(ogame);
        ogame=NULL;              
    }

    free(game_reply);
    game_reply=NULL;
    

}