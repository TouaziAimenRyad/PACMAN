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

#include "aux.h"

#ifndef JOUEUR_PENDANT_H_
#define JOUEUR_PENDANT_H_

void move_up(int connection_socket,int dist);
void move_down(int connection_socket,int dist);
void move_left(int connection_socket,int dist);
void move_right(int connection_socket,int dist);
void reply_move(int connection_socket);
void quit(int connection_socket);
void get_list_req(int connection_socket);
void get_list_res(int connection_socket);
void send_muilti_def_mail(int connection_socket ,char* message);
void send_private_msg(int connection_socket, char *id, char *message);


#endif /* AACE20D2_EF50_4531_A19A_D03573EF0633 */
