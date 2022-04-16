#include "req_aft_start.h"


void move_up(int connection_socket,int dist){   
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

void move_down(int connection_socket,int dist){   
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

void move_left(int connection_socket,int dist){   
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

void move_right(int connection_socket,int dist){   
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

void reply_move(int connection_socket){
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
void quit(int connection_socket){
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

void get_list(int connection_socket){
    if (send(connection_socket,"GLIS?***",8,0)<8)
    {
        perror("prolem while sending\n");
        close(connection_socket);
        exit(1);
    }
    

}

void get_list_res(int connection_socket){
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
/*
void muilti_def_mail(char * ip_multicast){

}
*/
