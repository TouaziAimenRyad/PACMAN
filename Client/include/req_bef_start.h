#ifndef REQ_BEF
#define REQ_BEF

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

void connection_reply(int connection_socket);
void regstr_reply(int connection_socket);
void reg_new_partie(int connection_socket,char *id ,char* udp_port);
void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game);
void unreg(int connection_socket);
void lab_size(int connection_socket,int partie);
void palyers_list(int connection_socket,int partie);
void partie_noncom_list(int connection_socket);
void start(int connection_socket);
void start_reply(int connection_socket);

#endif