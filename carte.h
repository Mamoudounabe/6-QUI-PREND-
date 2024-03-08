#ifndef CARTE
	#define CARTE

	typedef struct Carte{
		int num;
		int nbrTetesBoeufs;
		int joueurIndex;
		struct Carte* suivant;
	}Carte;
	Carte* ajouterCarte(Carte *carte,int num,int nbrTetes_boeufs,int joueurIndex);
	int supprimerCartes(Carte *carte);
	void afficherCartes();
	int choisirRangee(Carte *rangees[],int socketServeur);
	Carte* ajouterCarteRangee(Carte *rangee,Carte* carte);
	int nbrCartes(Carte *carte);
	Carte* prendreCarte(Carte **carte);
	Carte* ajouterFinCarte(Carte* carte,int num,int nbrTetesBoeufs,int joueurIndex);
#endif