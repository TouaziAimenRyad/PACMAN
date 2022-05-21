
#include "joueur_avant_deb.h"
//recevoir les informations en se connectant au serveur
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

//recevoir le message du serveur apres s'inscrire ou bien rejoindre une partie 
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

//demander de creer une partie
void reg_new_partie(int connection_socket,char *id ,char* udp_port,int type,uint16_t hauteur,uint16_t largeur){
    //id and udp port must be created using malloc so we can avoid them overflowing
    char * buff=malloc(29);
    int taille=22;
    if(type){
        sprintf(buff,"NEWLP %s %s hh ww***",id,udp_port);
        *(uint16_t*)(buff+20)=htons(hauteur);
        *(uint16_t*)(buff+23)=htons(largeur);
        taille=28;
    }else sprintf(buff,"NEWPL %s %s***",id,udp_port);
    if (send(connection_socket,buff,taille,0)<taille)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }

    free(buff);
}

//demander de rejoindre une partie
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

//demander de quitter sa partie
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

//demander la taille de labyrinthe
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

//demander la liste des joueurs d'une partie non commencé
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

//recevoir la liste des joueurs d'une partie non commencé
void partie_noncom_list(int connection_socket){
    if (send(connection_socket,"GAME?***",8,0)<8){
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    connection_reply(connection_socket);
}

//recevoir la liste des joueurs d'une partie commencé
void partie_com_reply(int connection_socket){
        void * buff=malloc(15);
        int inc=0;
        while (inc<13)
        {
            int r=recv(connection_socket,buff+inc,13-inc,0);
            if (r==-1)
            {
                perror("prolem while receiving\n");
                free(buff);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint32_t nbr_partie=htonl(*((uint32_t *)(buff+6))) ;
        printf("Games %d\n",nbr_partie);

        for (uint32_t i = 0; i <nbr_partie; i++)
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

//demander la liste des joueurs d'une partie commencé
void partie_com_list(int connection_socket){
    if (send(connection_socket,"GAMEC***",8,0)<8){
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    partie_com_reply(connection_socket);
}

//demander et recevoir la liste des parties et de leurs joueurs en donnant le numero de partie
void palyers_list_com(int connection_socket,int partie){
    void * buff=malloc(13);
    sprintf((char*)buff,"%s","LISTC n***");
    *((uint8_t*)(buff+6))=(uint8_t)partie;
    if (send(connection_socket,buff,10,0)<10)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }

    int inc=0;
    while (inc<13)
    {
        int r=recv(connection_socket,buff+inc,13-inc,0);
        if (r==-1)
        {
            perror("problem while receiving\n");
            free(buff);
            close(connection_socket);
            exit(1);
        }
        inc+=r;   
    }

    uint32_t nb_games= htonl(*((uint32_t *)(buff+6)));
    printf("Games %d\n",nb_games);
    for(uint32_t i=0;i<nb_games;i++){
        inc=0;
        while (inc<10)
        {
            int r=recv(connection_socket,buff+inc,10-inc,0);
            if (r==-1)
            {
                perror("problem while receiving\n");
                free(buff);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint8_t nb_player=*((uint8_t *)(buff+6));
        printf("nb joueurs=%u\n",nb_player);
        char * id;
        char * x;
        char * y;
        char * point;
        int size=30;
        char player_reply[size];
        for (int i = 0; i < nb_player; i++){
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
            id=player_reply+6;
            id[8]='\0';
            x=player_reply+15;
            x[3]='\0';
            y=player_reply+19;
            y[3]='\0';
            point=player_reply+23;
            point[4]='\0';
            printf("joueur %s in x= %d, y=%d with point= %d\n",id,atoi(x),atoi(y),atoi(point));
        }
    }                
    free(buff);
}

//changer le id si c'est possible
int change_id(int connection_socket,char* new_id){

    void * buff=malloc(18);
    sprintf((char*)buff,"CHAID %s***",new_id);
    if (send(connection_socket,buff,17,0)<17)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }

    int inc=0;
    while (inc<8)
    {
        int r=recv(connection_socket,buff+inc,8-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(buff);
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if(strncmp(buff,"CHAOK",5)==0){
        printf("id is changed\n");
        return 0;
    }
    else if(strncmp(buff,"CHANO",5)==0){
        printf("id is not changed\n");
        return 1;
    }
    return 1; 
}

//changer le port udp si c'est possible
int change_udp(int connection_socket,char* udp){

    void * buff=malloc(14);
    sprintf((char*)buff,"CHUDP %s***",udp);
    if (send(connection_socket,buff,13,0)<13)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }

    int inc=0;
    while (inc<8)
    {
        int r=recv(connection_socket,buff+inc,8-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(buff);
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if(strncmp(buff,"CHAOK",5)==0){
        printf("udp port is changed\n");
        return 0;
    }
    else if(strncmp(buff,"CHANO",5)==0){
        printf("udp port is not changed\n");
        return 1;
    }
    return 1;
}

