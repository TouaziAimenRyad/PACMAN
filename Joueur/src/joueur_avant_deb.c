
#include "joueur_avant_deb.h"
void connection_reply(int connection_socket){
        void * buff=malloc(12);
        int inc=0;
        while (inc<10)
        {
            int r=recv(connection_socket,buff+inc,10-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                free(buff);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint8_t nbr_partie=(*((uint8_t *)(buff+6))) ;
        printf("Games %d\n",nbr_partie);

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

void regstr_reply(int connection_socket){   
    char *buff=malloc(5);
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {
            perror("pbrolem while receiving\n");
            free(buff);
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
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        printf("refused\n");
    }
    else if(strncmp(buff,"REGOK",5)==0){
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
        free(buff);
        close(connection_socket);
        exit(1);
    }

    free(buff);
}

///////////////////////////////////////////////////////////////////////

void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game){
    void * buff=malloc(24);

    sprintf((char *)buff,"REGIS %s %s n***",id,udp_port);
    *((uint8_t *)(buff+20))=(uint8_t)game;
    if (send(connection_socket,buff,24,0)<24){
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }

    free(buff);
}

//////////////////////////////////////////

void unreg(int connection_socket){
    if (send(connection_socket,"UNREG***",8,0)<8){
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
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    if(strncmp(buff,"UNROK",5)==0){
        inc=0;
        while (inc<5){
            int r=recv(connection_socket,buff+inc,5-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                free(buff);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        printf("desinscription de partie num %d\n",*((uint8_t *)(buff+1)));
    }
    else if(strncmp(buff,"DUNNO",5)==0){
        printf("vous n'etes pas encore inscrit\n");
        inc=0;
        while (inc<3)
        {
            int r=recv(connection_socket,buff+inc,3-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                free(buff);
                close(connection_socket);
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
    sprintf((char*)buff,"%s","SIZE? n***");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    if (send(connection_socket,buff,10,0)<10){
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }

    int inc=0;
    while (inc<5){
        int r=recv(connection_socket,buff+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(buff);
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
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        uint16_t hauteur=ntohs(*((uint16_t *)(buff+3)));
        uint16_t largeur=ntohs(*((uint16_t *)(buff+6)));
        printf("hauteur:  %d   largeur :%d \n",hauteur,largeur);
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

void palyers_list(int connection_socket,int partie){
    void * buff=malloc(10);
    sprintf((char*)buff,"%s","LIST? n***");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        free(buff);
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
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }

        uint8_t nb_player= *((uint8_t *)(buff+3));
        printf("players: \n");
        char * player=malloc(17*nb_player);
        char* player_id;
        inc=0;
        while (inc<17*nb_player){
            int r=recv(connection_socket,player+inc,17*nb_player-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                free(buff);
                free(player);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        for (uint8_t i = 0; i < nb_player; i++){
            player_id=player+6+i*17;
            player_id[8]='\0'; 
            printf("\t player: %s\n",player_id); 
                             
        }
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
                free(buff);
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

void partie_noncom_list(int connection_socket){
    if (send(connection_socket,"GAME?***",8,0)<8){
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    connection_reply(connection_socket);
}