#include<stdio.h>
#include<stdlib.h>
#include"joueur.h"
#include"serveur.h"
Joueur* creerJoueur(const int socket,const struct sockaddr_in adresse,const char *pseudo){

	Joueur* joueur=(Joueur*)malloc(sizeof(Joueur));
	//joueur->cartes=ajouterCarte(NULL,1,1,0);
	joueur->cartes=NULL;
	joueur->score=0;
	sprintf(joueur->pseudo,"%s",pseudo);
	joueur->socket=socket;
	joueur->adresse=adresse;
	return joueur;
}


Joueur* supprimerJoueur(Joueur *joueur){
	if(joueur)
	{

		supprimerCartes(joueur->cartes);
		joueur->cartes=NULL;
		//if(joueur->socket!=0)
		//	close(joueur->socket);
		free(joueur);
		joueur=NULL;
	}
	return joueur;
}
void afficher(Utilisateur user){
	printf("pseudo:%s  %d\n",user.nom,user.score);
}
int choisirNum(Carte* carte,int socketServeur){
	Carte* ps=carte;
	int num=-1;
	afficherCartes(carte);
	do{
		printf("Choisir un num :");
		scanf("%d",&num);
		ps=carte;
		while(ps){

			if(ps->num==num)
				return num;
			
			ps=ps->suivant;
		}
	}while(1);
	//send(socketServeur,&num,sizeof(int),0);
	return num;
}
int demandeChoisirNum(int socketClient){
	int num=0;
	int option=1;
	send(socketClient,&option,sizeof(int),0);
	recv(socketClient,&num,sizeof(int),0);
	return num;
}
void* jouerCarte(void* _joueur){

	//printf("jouerCarte %d\n",joueur->socket);
	Joueur *joueur=(Joueur*)_joueur;
	int num=0;
	num=demandeChoisirNum(joueur->socket);
	//printf("jouerCarte\n");
	Carte* ps=joueur->cartes;
	if(ps->num==num)
	{
		joueur->cartes=ps->suivant;
		ps->suivant=NULL;
		return ps;
	}

	while(ps->suivant->num!=num)
	ps=ps->suivant;

	Carte *ps2=ps->suivant;
	ps->suivant=ps->suivant->suivant;
	ps2->suivant=NULL;
	return ps2;
}
void afficherJoueur(Joueur *joueur){
	printf("pseudo:%s    score: %d \n\n\t",joueur->pseudo,joueur->score);
	afficherCartes(joueur->cartes);
}
Joueur* recevoir(int socketClient){
	Joueur*  joueur=(Joueur*)malloc(sizeof(Joueur));
	joueur->cartes=(Carte*)malloc(sizeof(Carte));

	printf("1\n");
	recv(socketClient,joueur,sizeof(Joueur),0);
	//printf("%d \n",(int)recv(socketClient,joueur,sizeof(Joueur),0));
	//printf("2\n");
	//Carte c;
	//printf("%d \n",(int)recv(socketClient,joueur->cartes,sizeof(Carte),0));
	//printf("carte %d",joueur->cartes->num);
	//printf("fin\n");

		Carte *cartes=NULL;
		int tailleCarte;
	for(int i=0;i<tailleCarte;i++){
		int num,nbrTetesBoeufs,joueurIndex;
		recv(socketClient,&num,sizeof(int),0);
		recv(socketClient,&nbrTetesBoeufs,sizeof(int),0);
		recv(socketClient,&joueurIndex,sizeof(int),0);
		cartes=ajouterFinCarte(cartes,num,nbrTetesBoeufs,joueurIndex);
	}
	supprimerCartes(joueur->cartes);
	joueur->cartes=cartes;
	return joueur;
}
int recupNbrJoueur(const char *buffer){
	int i=0,x=0;
	while(buffer[i]!=',')
            i++;
       
    while(buffer[++i]!=',')
        x=x*10+toInt(buffer[i]);
    return x;
}
void synchronisationAvecServeur(Serveur **serveur,int socketServeur){
    FILE *fichier=fopen("myserveurUpdated.jeu","w");
    char buffer[400];
    int nbrJoueur=0;
    //printf("Dans synchro\n");
    if(recv(socketServeur,buffer,400,0)>0){
    	nbrJoueur=recupNbrJoueur(buffer);
    //	printf("%s\n",buffer);
        fprintf(fichier,"%s",buffer);
    }

    for(int i=0;i<nbrJoueur;i++){
    	recv(socketServeur,buffer,400,0);
      //  printf("%s\n",buffer);
        fprintf(fichier,"%s",buffer);
      }
	//}
     // printf("MM\n");
	int pret=100;
    fclose(fichier);
    // mise à jour de la variable serveur
     nettoyerServeur(*serveur);
    *serveur=deserialiserServeur("myserveurUpdated.jeu");
    send(socketServeur,&pret,sizeof(int),0);
   // printf("%d\n\n",pret);
}
int connexionAuServeur(const char *adresseip,const int port,int *indexJoueur){
	int socketServeur=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in adresse;
	if(socketServeur==0){
		printf("Echec lors de la création du socket client\n");
		return -1;
	}
	//192.168.11.110 /24

	adresse.sin_family=AF_INET;
	adresse.sin_addr.s_addr=inet_addr(adresseip);// on p
	adresse.sin_port=htons(port);

	if(connect(socketServeur,(const struct sockaddr*)&adresse,sizeof(struct sockaddr_in))<0){
			printf("Erreur lors de la connexion avec le serveur\n");
			return -1;
		}

	printf("Entrer votre pseudo:");
	char pseudo[25];
	scanf("%s",pseudo);
	send(socketServeur,pseudo,25,0);
	printf("________ \tEn attente des autres joueurs ... \t ____________\n\n");
	recv(socketServeur,indexJoueur,sizeof(int),0);
	//printf("Val index %d",*indexJoueur);
	int pret=20;
	send(socketServeur,&pret,sizeof(int),0);
	//printf("Sortie\n");
	return socketServeur;
 /*	Joueur  *joueur=recevoir(socketClient);
	//char pseudo[25];
	//recv(socketClient,&joueur,sizeof(Joueur),0);
	afficherJoueur(joueur);
	printf("pseudo : %s \t score:%d",joueur->pseudo,joueur->score);
	printf("En Attente des autres joueurs ....");
	int pret=0;
	recv(socketClient,&pret,sizeof(int),0);

	if(pret)
	{
		printf("Debut du jeu !!!\n");
		afficherJoueur(joueur);
	}*/
	//return socketClient;

}
/*
int main(int argc,char *argv[]){
	int nbrJoueurs=5;
	Joueur *joueurs[nbrJoueurs];

	for(int i=0;i<nbrJoueurs;i++)
		joueurs[i]=creerJoueur();

	
		for(int tour=0;tour<10;tour++)
			for(int i=0;i<nbrJoueurs;i++)
		joueurs[i]->cartes=AjouterCarte(joueurs[i]->cartes,10*tour+i,i%5);

	for(int i=0;i<nbrJoueurs;i++)
		afficherJoueur(joueurs[i]);
	for(int i=0;i<nbrJoueurs;i++)
		supprimerJoueur(joueurs[i]);
	supprimerJoueur();

	return 0;

}*/