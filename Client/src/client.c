
#include "client.h"


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