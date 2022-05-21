#include "joueur_pendant.h"

//demander un move up
void move_up(int connection_socket,int dist){
    char d[3];
    char buff[3];
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char req[12];
    sprintf(req,"UPMOV %s***",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("problem while sending\n");
        close(connection_socket);
        exit(1);
    }

}

//demander un move down
void move_down(int connection_socket,int dist){   
    char d[3];
    char buff[3];
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char req[12];
    sprintf(req,"DOMOV %s***",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
}

//demander un move left
void move_left(int connection_socket,int dist){   
    char d[3];
    char buff[3];
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"LEMOV %s***",d);
    if (send(connection_socket,req,12,0)<12){
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
}

//demander un move rigth
void move_right(int connection_socket,int dist){   
    char d[3];
    char buff[3];
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"RIMOV %s***",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
}

//recevoir la reponse du serveur pour un move
int reply_move(int connection_socket){
    char reply_msg[5];
    int inc =0;
    while (inc<5){
        int r=recv(connection_socket,reply_msg+inc,5-inc,0);
        if (r==-1){   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r; 
    }

    if(strncmp("MOVE!",reply_msg,5)==0){
        char reply_mv[11];
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
        char* new_x=reply_mv+1;
        new_x[3]='\0';
        char* new_y=reply_mv+5;
        new_y[3]='\0';
        printf("position x= %d, y=%d\n",atoi(new_x),atoi(new_y));

    }
    else if(strncmp("MOVEF",reply_msg,5)==0){
        char reply_mvf[16];
        inc =0;
        while (inc<16){
            int r=recv(connection_socket,reply_mvf+inc,16-inc,0);
            if (r==-1){   
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        char* new_x=reply_mvf+1;
        new_x[3]='\0';
        char* new_y=reply_mvf+5;
        new_y[3]='\0';
        char* point=reply_mvf+9;
        point[4]='\0';

        printf("position x= %d, y=%d,points=%d\n",atoi(new_x),atoi(new_y),atoi(point));
    }else if(strncmp("GOBYE",reply_msg,5)==0){
        char bye[3];
        inc =0;
        while (inc<3){
            int r=recv(connection_socket,bye+inc,3-inc,0);
            if (r==-1){   
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        printf("goodbye\n");
        close(connection_socket);
        return 1;
    }
    return 0;
}

//demander de quitter une partie
void quit(int connection_socket){
    if (send(connection_socket,"IQUIT***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    char reply[8];
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
    if(strncmp("GOBYE",reply,5)==0){
        printf("goodbye\n");
        close(connection_socket);
    }
}

//demander la liste des joueurs de la partie
void get_list_req(int connection_socket)
{
    if (send(connection_socket,"GLIS?***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
}

//recevoir la liste des joueurs de la partie 
int get_list_res(int connection_socket){
    char reply[5];
    int inc =0;
    while (inc<5)
    {
        int r=recv(connection_socket,reply+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if(strncmp(reply,"GLIS!",5)==0)
    {
        char glis[5];
        inc =0;
        while (inc<5)
        {
            int r=recv(connection_socket,glis+inc,5-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint8_t nb_player=*(uint8_t *)(glis+1);
        printf("nb joueurs=%u\n",nb_player);
        char * id;
        char * x;
        char * y;
        char * point;
        int size=30;
        char player_reply[size];
        for (int i = 0; i < nb_player; i++)
        {
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
    else
    {
        if(strncmp("GOBYE",reply,5)==0)
        {
            char bye[3];
            inc =0;
            while (inc<3)
            {
                int r=recv(connection_socket,bye+inc,3-inc,0);
                if (r==-1)
                    {   
                        perror("prolem while receiving\n");
                        close(connection_socket);
                        exit(1);
                    }
                inc+=r;    
            }
            printf("goodbye\n");
            close(connection_socket);
            return 1;
        }
    }  
    return 0; 
}

//envoyer un message multicast
int send_muilti_def_mail(int connection_socket,char* message){      
    int msg_size=strlen(message);
    char *buff= malloc(msg_size+9);
    sprintf((char*) buff,"MALL? %s***",message);
    if (send(connection_socket,buff,msg_size+9,0)<msg_size+9)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }
    free(buff);

    char server_reply[8];
    int inc =0;
    while (inc<8) // the server will reply with either gobye! or mail!
    {
        int r=recv(connection_socket,server_reply+inc,8-inc,0);
        if (r==-1)
        {      
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
   
    if(!strncmp(server_reply,"MALL!",5))
    {
        printf("mail sent\n");
    }

    else if(!strncmp(server_reply,"GOBYE",5))
    {
        close(connection_socket);
        printf("goodbye \n");
        return 1;
    }
    else if(!strncmp(server_reply,"DUNNO",5))
    {
        printf("message not sent \n");
    }
    return 0;
}

//envoyer un message udp
int send_private_msg(int connection_socket,char* id,char *message){
    int msg_size=strlen(message);
    void *buff= malloc(msg_size+18);
    sprintf((char*) buff,"SEND? %s %s***",id,message);

    if (send(connection_socket,buff,msg_size+18,0)<msg_size+18)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }
    free(buff);

   
    char server_reply[8];
    int inc =0;
    while (inc<8) 
    {
        int r=recv(connection_socket,server_reply+inc,8-inc,0);
        if (r==-1)
        {      
            perror("prolem while receiving\n");
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    if(!strncmp(server_reply,"SEND!",5))
    {
        printf("message sent \n");
    }    
    else if(!strncmp(server_reply,"NSEND",5))
    {
        printf("can't send msg \n");
    }
    else if(!strncmp(server_reply,"GOBYE",5))
    {
        close(connection_socket);
        printf("goodbye\n");
        return 1;
    }
    return 0;
}

//cheat afficher labyrinthe
int afficher_labyrinthe(int connection_socket){
    void * buff=malloc(9);
    void * buff2=malloc(12);
    sprintf((char*)buff,"PRINT***");
    if (send(connection_socket,buff,8,0)<8)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff2+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(buff2);
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if(!strncmp(buff2,"GOBYE",5))
    {
        close(connection_socket);
        printf("goodbye\n");
        return 1;
    }
    if(strncmp(buff2,"LABYR",5)==0){
        int inc=0;
        while (inc<7)
        {
            int r=recv(connection_socket,buff2+inc,7-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                free(buff2);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint16_t hauteur=ntohs(*(uint16_t*)(buff2+1))+2;
        uint16_t largeur=ntohs(*(uint16_t*)(buff2+4))*2+1;
        int taille=hauteur*largeur+hauteur;
        void *buff3=malloc(taille);
        inc=0;
        while (inc<taille-1){
            int r=recv(connection_socket,buff3+inc,taille-1-inc,0);
                if (r==-1){   
                    perror("prolem while receiving\n");
                    free(buff3);
                    close(connection_socket);
                    exit(1);
                }
            inc+=r;   
        }
        for(uint16_t i=0;i<hauteur;i++){
            *(char*)(buff3+largeur)='\0';
            printf("%s\n",(char*)buff3);
            buff3=buff3+largeur+1;
        }
    }
    return 0;
}

//cheat afficher la position des fontomes
int ghost_positions(int connection_socket){
    void * buff=malloc(9);
    void * buff2=malloc(5);
    sprintf((char*)buff,"GHOPS***");
    if (send(connection_socket,buff,8,0)<8)
    {
        perror("prolem while sending\n");
        free(buff);
        close(connection_socket);
        exit(1);
    }
    int inc=0;
    while (inc<5)
    {
        int r=recv(connection_socket,buff2+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(buff2);
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if (!strncmp(buff2,"GOBYE",5))
    {
        close(connection_socket);
        printf("goodbye\n");
        return 1;
    } 
    if(strncmp(buff2,"GHORE",5)==0){
        int inc=0;
        while (inc<5)
        {
            int r=recv(connection_socket,buff2+inc,5-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                free(buff2);
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint8_t nb_fontomes=*(uint16_t*)(buff2+1);
        int taille=16*nb_fontomes;
        void *buff3=malloc(taille);
        inc=0;
        while (inc<taille){
            int r=recv(connection_socket,buff3+inc,taille-inc,0);
                if (r==-1){   
                    perror("prolem while receiving\n");
                    free(buff3);
                    close(connection_socket);
                    exit(1);
                }
            inc+=r;   
        }
        char *x,*y;
        for(uint16_t i=0;i<nb_fontomes;i++){
            x=buff3+6;
            y=buff3+10;
            x[3]='\0';
            y[3]='\0';
            printf("fontome à la position :x=%s y=%s\n",(char*)x,(char*)y);
            buff3=buff3+16;
        }
    }
    return 0; 
}