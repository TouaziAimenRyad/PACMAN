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

#ifndef JOUEUR_MULTICAST_H_
#define JOUEUR_MULTICAST_H_

void abonner_multi(char * ip_multicast,char* port_multicast);//after start reply we call this to inter the multicast ring
void recv_muilti_def(int multicast_sock);


#endif /* A1E10F8F_3F6D_4834_9495_0D42C5D81F27 */
