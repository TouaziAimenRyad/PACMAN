Projet Réseau

Trinôme:
    HABLAL Abdelhamid
    TOUKAL Raouf
    Touazi Aimen Ryad  

	Le code du serveur est écris en java pour mieux gerer les classes et la synchronization
    et celle des joueurs en C.

Tester :

	***Pour tester lors de la soutenance*** 
	-Décompressez l'archive.
	-Ouvrez un terminal dans ce même répertoire.
	-Exécutez la commande "make" (cela va compiler tout les fichiers .java et .c)
	-Dans un deuxime terminal placez vous dans le dossier Serveur et exécuter la commande "java Serveur <port>"
	-Dans le premier terminal (autre machine) exécutez "./joueur <votre port UDP> "
	C'est partit !

Contenance :
	
	- [Dossier] Serveur
        - [Fichier] Serveur.java (le main du serveur) 
        - [Fichier] Comm_client.java (le thread qui gère la communication avec le joueur)
        - [Fichier] External.java (class qui contient les differents entete des messages)
        - [Fichier] Labyrinthe.java (class qui gère notre labyrinthe)
        - [Fichier] Partie.java (class qui gère la partie)
        - [Fichier] Player.java (class qui symbolise le joueur)
        - [Fichier] Partie_extension.java (class fils de Partie.java qui gere les Extensions)
    - [Dossier] Joueur 
        - [Dossier] src
            - [Fichier] aux.c (fichier qui contient des fonctions auxiliaires )
            - [Fichier] joueuer_multicast.c (abonner et recevoir des messages multicast )
            - [Fichier] joueur_avant_deb.c (fonctionalités avant de commencer une partie)
            - [Fichier] joueur_debut.c (le start)
            - [Fichier] joueur_pendant.c (fonctionalités pendant une partie)
            - [Fichier] joueur_udp.c (recevoir des messaegs udp)
        - [Dossier] includes
            - [Fichier] aux.h
            - [Fichier] joueuer_multicast.h
            - [Fichier] joueur_avant_deb.h
            - [Fichier] joueur_debut.h
            - [Fichier] joueur_pendant.h
            - [Fichier] joueur_udp.h
            - [Fichier] joueur.h
        - [Fichier] joueur.c (le main du programe c)
	- [Fichier] Makefile
	- [Fichier] README.txt

***Projet classic***
on a implementer toutes les fonctionalités demandés ou on a gèrer tout ce qu'on peut imaginer.
notre code java est bien structuré et gere tres bien les accès concurent sur les variables partagés.
notre code en c est bien structuré  
***Extensions***
en plus de ce qui est demandé on a rajouté:

avant de commencer une partie:
    changer le port udp (si c'est possible)
    changer le id (si c'est possible)
    la liste des parties commencés
    pour un numero de partie commencé la liste des joueurs avec leurs informations(id,x,y,score)
    ceer une partie special

pendant une partie special:
    on peut si c'est possible passer par les bords de la labyrinthe et sortire de l'autre bord
    les fontomes bouge d'une facon differente de la facon classic par un deplacement instantané(son goku)
    Malgre que ca ne s'affiche pas dans le menu mais y'a des codes de triches:
        9-pour afficher le labyrinthe
        10-pour afficher la positions des fantomes non capturés