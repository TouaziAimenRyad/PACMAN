#include "joueur_pendant.h"

void move_up(int connection_socket,int dist)
{   
    printf("move up\n");
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","UPMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("problem while sending\n");
        free(d);
        d=NULL;
        free(buff);
        buff=NULL;
        free(req);
        req=NULL;
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

void move_down(int connection_socket,int dist)
{   
    printf("move down\n");
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","DOMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        free(d);
        d=NULL;
        free(buff);
        buff=NULL;
        free(req);
        req=NULL;
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
    printf("move left\n");
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","LEMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        free(d);
        d=NULL;
        free(buff);
        buff=NULL;
        free(req);
        req=NULL;
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
    printf("move right\n");
    char * d=malloc(3);
    char *buff=malloc(3);
    sprintf(buff,"%d",dist);
    PrependZeros(d,buff,3);

    char * req= malloc(12);
    sprintf(req,"%s %s***","RIMOV",d);
    if (send(connection_socket,req,12,0)<12)
    {
        perror("prolem while sending\n");
        free(d);
        d=NULL;
        free(buff);
        buff=NULL;
        free(req);
        req=NULL;
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

//bdlnaha
void reply_move(int connection_socket)
{
    printf("reply move\n");
    char * reply_msg=malloc(5);
    int inc =0;
    while (inc<5)
    {
        int r=recv(connection_socket,reply_msg+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(reply_msg);
            reply_msg=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r; 
    }

    if(strncmp("MOVE!",(char*)reply_msg,5)==0){
        void *reply_mv=malloc(11);
        inc =0;
        while (inc<11)
        {
            int r=recv(connection_socket,reply_mv+inc,11-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                free(reply_mv);
                reply_mv=NULL;
                free(reply_msg);
                reply_msg=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        char* new_x=malloc(3);
        char* new_y=malloc(3);
        strncpy(new_x,(char*)(reply_mv+1),3);
        strncpy(new_y,(char*)(reply_mv+1+4),3);
        printf("x= %d, y=%d\n",atoi(new_x),atoi(new_y));
        free(new_x);new_x=NULL;
        free(new_y);new_y=NULL;
        free(reply_mv);
        reply_mv=NULL;

    }
    else if(strncmp("MOVEF",(char*)reply_msg,5)==0){
        void *reply_mvf=malloc(16);
        inc =0;
        while (inc<16){
            int r=recv(connection_socket,reply_mvf+inc,16-inc,0);
            if (r==-1){   
                perror("prolem while receiving\n");
                free(reply_mvf);
                reply_mvf=NULL;
                free(reply_msg);
                reply_msg=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        char* new_x=malloc(3);
        char* new_y=malloc(3);
        char* point=malloc(4);
        strncpy(new_x,(char*)(reply_mvf+1),3);
        strncpy(new_y,(char*)(reply_mvf+1+4),3);
        strncpy(point,(char*)(reply_mvf+1+4+4),4);

        printf("x= %d, y=%d, p=%d\n",atoi(new_x),atoi(new_y),atoi(point));
        free(new_x);new_x=NULL;
        free(new_y);new_y=NULL;
        free(point);point=NULL;
        free(reply_mvf);
        reply_mvf=NULL;
    }else if(strncmp("GOBYE",(char*)reply_msg,5)==0){
        void *bye=malloc(3);
        inc =0;
        while (inc<3){
            int r=recv(connection_socket,bye+inc,3-inc,0);
            if (r==-1){   
                perror("prolem while receiving\n");
                free(bye);
                bye=NULL;
                free(reply_msg);
                reply_msg=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        free(bye);
        bye=NULL;
        close(connection_socket);
    }

    free(reply_msg);
    reply_msg=NULL;

}

///////////////////////////////////
void quit(int connection_socket)
{
    printf("quit\n");
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
            free(reply);
            reply=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }

    free(reply);
    reply=NULL;
}


/////////////////////////////////////////////////

void get_list_req(int connection_socket)
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
    printf("get list while playing\n");
    char* reply=malloc(5);
    int inc =0;
    while (inc<5)
    {
        int r=recv(connection_socket,reply+inc,5-inc,0);
        if (r==-1)
        {   
            perror("prolem while receiving\n");
            free(reply);
            reply=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
    if(strncmp(reply,"GLIS!",5)==0)
    {
        void * glis=malloc(5);
        inc =0;
        while (inc<5)
        {
            int r=recv(connection_socket,glis+inc,5-inc,0);
            if (r==-1)
            {   
                perror("prolem while receiving\n");
                free(reply);
                reply=NULL;
                free(glis);
                glis=NULL;
                close(connection_socket);
                exit(1);
            }
            inc+=r;    
        }
        uint8_t nb_player=*(uint8_t *)(glis+1);
        printf("nb joueurs=%u\n",nb_player);
        char * id=malloc(8);
        char * x=malloc(3);
        char * y=malloc(3);
        char * point=malloc(4);
        int size=6+9+4+4+4+3;
        void * player_reply=malloc(size);
        for (int i = 0; i < nb_player; i++)
        {
            inc =0;
            while (inc<size)
            {
                int r=recv(connection_socket,player_reply+inc,size-inc,0);
                if (r==-1)
                {      
                    perror("prolem while receiving\n");
                    free(reply);
                    reply=NULL;
                    free(glis);
                    glis=NULL;
                    free(player_reply);
                    player_reply=NULL;
                    close(connection_socket);
                    exit(1);
                }
                inc+=r;    
            }
            strncpy(id,(char*)(player_reply+6),8);
            strncpy(x,(char*)(player_reply+6+9),3);
            strncpy(y,(char*)(player_reply+6+9+4),3);
            strncpy(point,(char*)(player_reply+6+9+4+4),4);
            printf("joueur %s in x= %d, y=%d with point= %d\n",id,atoi(x),atoi(y),atoi(point));
        }
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
        free(glis);
        glis=NULL;
    }
    else
    {
        if(strncmp("GOBYE",(char*)reply,5)==0)
        {
            void *bye=malloc(3);
            inc =0;
            while (inc<3)
            {
                int r=recv(connection_socket,bye+inc,3-inc,0);
                if (r==-1)
                    {   
                        perror("prolem while receiving\n");
                        free(bye);
                        bye=NULL;
                        free(reply);
                        reply=NULL;
                        close(connection_socket);
                        exit(1);
                    }
                inc+=r;    
            }
            free(bye);
            bye=NULL;
            close(connection_socket);

        }
    }   

    free(reply);
    reply=NULL;

}




void send_muilti_def_mail(int connection_socket,char* message) //send to the server than the server wil do the multicast
{   
    printf("send mail while playing\n");    
    int msg_size=strlen(message);
    void *buff= malloc(msg_size+9);
    sprintf((char*) buff,"%s %s***","MAIL?",message);
    if (send(connection_socket,buff,msg_size+9,0)<msg_size+9)
    {
        perror("prolem while sending\n");
        free(buff);
        buff=NULL;
        close(connection_socket);
        exit(1);
    }
    free(buff);
    buff=NULL;

    char *server_reply=malloc(8);
    int inc =0;
    while (inc<8) // the server will reply with either gobye! or mail!
    {
        int r=recv(connection_socket,server_reply+inc,8-inc,0);
        if (r==-1)
        {      
            perror("prolem while receiving\n");
            free(server_reply);
            server_reply=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
   
    
    if(strncmp(server_reply,"GOBYE",5))
    {
        close(connection_socket);
        printf("goodbye  %s\n",(char *)(server_reply));
    }
    free(server_reply);
    server_reply=NULL;



}

//udp

void send_private_msg(int connection_socket,char* id,char *message)
{
    printf("send private msg while playing\n");    
    int msg_size=strlen(message);
    void *buff= malloc(msg_size+18);
    sprintf((char*) buff,"%s %s %s***","SEND?",id,message);

    if (send(connection_socket,buff,msg_size+18,0)<msg_size+18)
    {
        perror("prolem while sending\n");
        free(buff);
        buff=NULL;
        close(connection_socket);
        exit(1);
    }
    free(buff);
    buff=NULL;

   
    char *server_reply=malloc(8);
    int inc =0;
    while (inc<8) 
    {
        int r=recv(connection_socket,server_reply+inc,8-inc,0);
        if (r==-1)
        {      
            perror("prolem while receiving\n");
            free(server_reply);
            server_reply=NULL;
            close(connection_socket);
            exit(1);
        }
        inc+=r;    
    }
   
    
    if(strncmp(server_reply,"SEND!",5))
    {
        close(connection_socket);
        printf("message sent  %s\n",(char *)(server_reply));
    }
    
    if(strncmp(server_reply,"NSEND",5))
    {
        close(connection_socket);
        printf("can't send msg  %s\n",(char *)(server_reply));
    }

    if(strncmp(server_reply,"GOBYE",5))
    {
        close(connection_socket);
        printf("goodbye  %s\n",(char *)(server_reply));
    }
    
    
    free(server_reply);
    server_reply=NULL;


}