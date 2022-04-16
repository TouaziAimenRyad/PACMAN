#ifndef REQ_AFT
#define REQ_AFT

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
#include "functions_aux.h"

void move_up(int connection_socket,int dist);
void move_down(int connection_socket,int dist);
void move_left(int connection_socket,int dist);
void move_right(int connection_socket,int dist);
void move_reply(int connection_socket);
void quit(int connection_socket);
void get_list_req(int connection_socket);
void get_list_res(int connection_socket);

#endif