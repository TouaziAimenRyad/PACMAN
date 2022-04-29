  CC=gcc
  JV=javac
  INCLUDE=Joueur/includes
  CFLAGS+= -g -o1 -Wall -Wextra -pthread -I $(INCLUDE)
  
  all : joueur Serveur.class

  joueur : aux.o joueur.o joueuer_multicast.o joueur_udp.o joueur_avant_deb.o joueur_debut.o joueur_pendant.o
	$(CC) $(CFLAGS) -o joueur joueur.o joueuer_multicast.o joueur_udp.o joueur_avant_deb.o aux.o joueur_debut.o joueur_pendant.o
  
  joueur.o : Joueur/joueur.c
	$(CC) $(CFLAGS) -o joueur.o -c Joueur/joueur.c

  joueur_avant_deb.o : Joueur/src/joueur_avant_deb.c
	$(CC) $(CFLAGS) -o joueur_avant_deb.o -c Joueur/src/joueur_avant_deb.c

  joueur_debut.o : Joueur/src/joueur_debut.c
	$(CC) $(CFLAGS) -o joueur_debut.o -c Joueur/src/joueur_debut.c

  joueur_pendant.o : Joueur/src/joueur_pendant.c
	$(CC) $(CFLAGS) -o joueur_pendant.o -c Joueur/src/joueur_pendant.c

  joueuer_multicast.o : Joueur/src/joueuer_multicast.c
	$(CC) $(CFLAGS) -o joueuer_multicast.o -c Joueur/src/joueuer_multicast.c

  joueur_udp.o : Joueur/src/joueur_udp.c
	$(CC) $(CFLAGS) -o joueur_udp.o -c Joueur/src/joueur_udp.c

  aux.o : Joueur/src/aux.c
	$(CC) $(CFLAGS) -o aux.o -c Joueur/src/aux.c

  Serveur.class : Serveur/Serveur.java
	$(JV) -cp Serveur/ Serveur/Serveur.java
  
  distclean : 
	rm -rf *.o
