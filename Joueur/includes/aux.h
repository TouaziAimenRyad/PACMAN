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

#ifndef AUX_H_
#define AUX_H_
void PrependZeros(char *dest, const char *src, unsigned minimal_width);
char* remove_hashtags(char* src);

#endif 
