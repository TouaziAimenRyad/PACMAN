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

#ifndef JOUEUR_DEB_H_
#define JOUEUR_DEB_H_

void start(int connection_socket);
void start_reply(int connection_socket);



#endif /* E95B3004_4127_44CA_A5B9_44B192580BE7 */
