#include "req_bef_start.h"

void connection_reply(int connection_socket){
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

void regstr_reply(int connection_socket){   
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

void reg_new_partie(int connection_socket,char *id ,char* udp_port){
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

void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game){
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

void unreg(int connection_socket){
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

void lab_size(int connection_socket,int partie){

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

void palyers_list(int connection_socket,int partie){
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

void partie_noncom_list(int connection_socket){
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

void start(int connection_socket){
    if (send(connection_socket,"START***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
}

void start_reply(int connection_socket){
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