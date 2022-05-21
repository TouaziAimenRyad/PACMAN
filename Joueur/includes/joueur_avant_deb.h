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

#ifndef JOUEUR_AVANT_DEB_H_
#define JOUEUR_AVANT_DEB_H_


void connection_reply(int connection_socket);
void regstr_reply(int connection_socket);
void reg_new_partie(int connection_socket,char *id ,char* udp_port,int type,uint16_t hauteur,uint16_t largeur);
void reg_partie_existant(int connection_socket,char *id ,char* udp_port,int game);
void unreg(int connection_socket);
void lab_size(int connection_socket,int partie);
void palyers_list(int connection_socket,int partie);
void partie_noncom_list(int connection_socket);
void partie_com_list(int connection_socket);
void partie_com_reply(int connection_socket);
void palyers_list_com(int connection_socket,int partie);
int change_id(int connection_socket,char* new_id);
int change_udp(int connection_socket,char* udp);


#endif /* F4EBE56E_C953_4C56_8CED_EDC576A0629B */
