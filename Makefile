  CC=gcc
  JV=javac
  INCLUDE=Client/include
  CFLAGS+= -g -o1 -Wall -Wextra -pthread -I $(INCLUDE)
  
  all : client Serveur.class

  client : functions_aux.o client.o req_aft_start.o req_bef_start.o
	$(CC) -o client client.o req_aft_start.o req_bef_start.o functions_aux.o
  
  client.o : Client/src/client.c
	$(CC) $(CFLAGS) -o client.o -c Client/src/client.c

  req_aft_start.o : Client/src/req_aft_start.c
	$(CC) $(CFLAGS) -o req_aft_start.o -c Client/src/req_aft_start.c

  req_bef_start.o : Client/src/req_bef_start.c
	$(CC) $(CFLAGS) -o req_bef_start.o -c Client/src/req_bef_start.c

  functions_aux.o : Client/src/functions_aux.c
	$(CC) $(CFLAGS) -o functions_aux.o -c Client/src/functions_aux.c

  Serveur.class : Serveur/Serveur.java
	$(JV) -cp Serveur/ Serveur/Serveur.java
  
  distclean : 
	rm -rf *.o
