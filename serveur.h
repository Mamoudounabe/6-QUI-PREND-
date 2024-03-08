#ifndef SERVEUR
	#define SERVEUR
	#include"joueur.h"
	#include <arpa/inet.h>
	#include <pthread.h>
	// Serveur

	typedef struct Serveur{
		char description[100];
		Joueur **joueurs;
		Carte *cartes;
		Carte *rangees[4];
		char adresseIp[20];
		int port;
		int nbrJoueurs;
		int socket;
		struct sockaddr_in adresse;
	}Serveur;

	Serveur* creationPartie();
	Carte* creationCartes();
	Carte* melangerCartes(Carte* cartes);
	void afficherPartie(Serveur* serveur);
	void distribuerCarte(Serveur *serveur,int joueurIndex);
	void deposerCarteRangee(Serveur *serveur,Carte *carte);
	void tour(Serveur *serveur);
	int manche(Serveur *serveur);
	int nouveauTour(Serveur *serveur);
	Carte* rassemblerCarte(Carte *cartes,Carte* carte);
	void supprimerPartie(Serveur *serveur);
	void nettoyerServeur(Serveur* serveur);
	void afficherScores(Serveur *serveur);
	void afficherRangees(Carte *rangees[]);
	void initialiserSocketServeur(int *socket,struct sockaddr_in *adresse,const char *,const int);
	int accepterJoueur(Serveur *);
	void getConfig(Serveur *serveur);
	void misAjourServeur(Serveur *serveur);
	void envoyer(Joueur *joueur);
	int toInt(char c);
	void synchronisationAvecServeur(Serveur **,int socketServeur);
	Serveur* deserialiserServeur(const char *chemin);
	void* demandeChoisirRangee(void* socketClient);
	void* demandeAfficherScores(void *socketClient);
#endif