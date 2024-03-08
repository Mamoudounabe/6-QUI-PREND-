#include<stdio.h>
#include <stdlib.h>
#include<arpa/inet.h>
#include<unistd.h>
#include"carte.h"

Carte* ajouterCarte(Carte* carte,int num,int nbrTetesBoeufs,int joueurIndex){
	Carte* nouvelleCarte= (Carte*) malloc(sizeof(Carte));

	nouvelleCarte->num=num;
	nouvelleCarte->nbrTetesBoeufs=nbrTetesBoeufs;
	nouvelleCarte->joueurIndex=joueurIndex;
	nouvelleCarte->suivant=carte;

	return nouvelleCarte;
}

Carte* ajouterFinCarte(Carte* carte,int num,int nbrTetesBoeufs,int joueurIndex){
	Carte* nouvelleCarte= (Carte*) malloc(sizeof(Carte));

	nouvelleCarte->num=num;
	nouvelleCarte->nbrTetesBoeufs=nbrTetesBoeufs;
	nouvelleCarte->joueurIndex=joueurIndex;
	nouvelleCarte->suivant=NULL;
	if(!carte)
		return nouvelleCarte;

	Carte* ps=carte;
	while(ps->suivant)
		ps=ps->suivant;

	ps->suivant=nouvelleCarte;
	return carte;
}

int choisirRangee(Carte *rangees[],int socketServeur){
	
	for(int i=0;i<4;i++)
		afficherCartes(rangees[i]);

	int indexRangee=0;
	do{
		printf("Choisir une rangée:");
		scanf("%d",&indexRangee);
		indexRangee--; // pour que ça soit entre 0 et 3
	}while(indexRangee<0 || indexRangee>3);

	return indexRangee;
}

Carte* prendreCarte(Carte **cartes){
	Carte* ps=(*cartes);
	(*cartes)=ps->suivant;
	ps->suivant=NULL;
	return ps;
}
Carte* ajouterCarteRangee(Carte *rangee,Carte* carte){
	carte->suivant=rangee;
	return carte;
}

int nbrCartes(Carte *carte){
	int nbr=0;
	while(carte)
	{
		carte=carte->suivant;
		nbr++;
	}
	return nbr;
}

int supprimerCartes(Carte *carte){
	Carte *ps=NULL;
	int nbrTetesBoeufs=0;
	while(carte){
		ps=carte->suivant;
		nbrTetesBoeufs+=carte->nbrTetesBoeufs;
		free(carte);
		carte=ps;
	}
	return nbrTetesBoeufs;
}

void afficherCartes(Carte *carte){
	while(carte){
	printf("%2d-->%4d |",carte->num,carte->nbrTetesBoeufs);
	carte=carte->suivant;
	}
	printf("\n");

}
/*
int main(int argc,char** argv){
	Carte *carte=NULL;
	carte=ajouterCarte(carte,14,5,-1);
	afficherCartes(carte);
	supprimerCartes(carte);

	return 0;
}*/