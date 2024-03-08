#ifndef JOUEUR
	#define JOUEUR
	#include <arpa/inet.h>
	#include <unistd.h>
	#include<pthread.h>
	#include"carte.h"
	typedef struct Utilisateur{
		char nom[25];
		int score;
	}Utilisateur;
	typedef struct Joueur{
		char pseudo[25];
		Carte* cartes;
		int score;
		int socket;
		struct sockaddr_in adresse;
	}Joueur;

	Joueur* creerJoueur(const int socket,const struct sockaddr_in,const char *pseudo);
	Joueur* supprimerJoueur(Joueur* joueur);
	void* jouerCarte(void*);
	void afficherJoueur(Joueur* joueur);
	void afficher(Utilisateur user);
	void afficherCartesJoueur(Joueur *joueur);
	int choisirNum(Carte* carte,int socketServeur);
	int choisirRangee(Carte **rangees,int socketServeur);
	int connexionAuServeur(const char *adresseip,const int port,int *indexJoueur);
	Joueur* recevoir(int );
	int demandeChoisirNum(int socketClient);
#endif