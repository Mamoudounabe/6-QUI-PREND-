#include<stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include"serveur.h"

/*typedef struct Joueur
{
	char nom[25];
	int score;
}Joueur;
void afficher(Joueur *joueur){
	printf("Joueur nom:%s \t score:%d\n",joueur->nom,joueur->score);
}
/*int main(int argc,char *argv[]){

		int socketClient=socket(AF_INET,SOCK_STREAM,0);

		if(socketClient==0){
			printf("Erreur lors de la creation du socket client");
			return -1;
		}

		struct sockaddr_in addrClient;
		//192.168.11.110 /24
		addrClient.sin_addr.s_addr=inet_addr("127.0.0.1");
		addrClient.sin_family=AF_INET;
		addrClient.sin_port=htons(20000);

		if(connect(socketClient,(const struct sockaddr*)&addrClient,sizeof(addrClient))<0){
			printf("Erreur lors de la connexion avec le serveur\n");
			return -2;
		}


		Joueur j1={.nom="VAN",.score=25};
		//send(socketClient,&j1,sizeof(Joueur),0);
		//afficher(&j1);
		Joueur joueur;
		recv(socketClient,&joueur,sizeof(Joueur),0);
		afficher(&joueur);
		close(socketClient);

		return 0;
}*/

int main(int argc,char *argv){

	Serveur* serveur=(Serveur*)malloc(sizeof(Serveur));
	int indexJoueurActu,option;
	getConfig(serveur);
	printf("\n-----\t Bienvenu(e) Dans le jeu Le 6 qui prend\t----------\n\n");
	printf("\n\t %s \n\n",serveur->description);
	int socketServeur=connexionAuServeur(serveur->adresseIp,serveur->port,&indexJoueurActu);
	//printf("je suis à la fin\n");
	//srecv(socketServeur,&val,sizeof(int),0);
	//printf("La valeur retournée est %d\n",val);
	int first=1;
	do{
		int option=5;
		int retourne=recv(socketServeur,&option,sizeof(int),0);
		if(retourne<0)
			return 1;
		else if (retourne==0)
			option=5;
		switch(option){
		  case 1: 	int num=choisirNum(serveur->joueurs[indexJoueurActu]->cartes,socketServeur);
		  			printf("%d\n\n",num);
		  			send(socketServeur,&num,sizeof(int),0);
		  			break;
		  case 2:
		  			int indexrangee=choisirRangee(serveur->rangees,socketServeur);
		  			send(socketServeur,&indexrangee,sizeof(int),0);
		  			break;
		  case 3:	
		  			sleep(0.01);
		  			synchronisationAvecServeur(&serveur,socketServeur);
		  			break;
		  case 4:
		  			int ack=1;
		  			afficherScores(serveur);
		  			send(socketServeur,&ack,sizeof(int),0);
		  			break; 
		}

	}while(option!=5);
	afficherScores(serveur);
	supprimerPartie(serveur);
	//choisirNum(serveur->joueurs[indexJoueurActu]->cartes,socketServeur);
	//recv(socketServeur,&option,sizeof(int),0);
	//afficherPartie(serveur);
	//printf("\n\n");
	///afficherJoueur(serveur->joueurs[indexJoueurActu]);
	//printf("------------\n");
	//afficherJoueur(joueur);
	return 0;
}