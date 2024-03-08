#include<stdio.h>
#include <stdlib.h>
#include<time.h>
#include"serveur.h"
#include <string.h>
Carte* creationCartes(){

	Carte *cartes=NULL;
	int nbrTetesBoeufs=1;
	for(int i=1;i<=104;i++){
		if(i%10==0)
			nbrTetesBoeufs=3;
		else if(i==55)
			nbrTetesBoeufs=7;
		else if(i%5==0)
			nbrTetesBoeufs=2;
		else if(i%11==0)
			nbrTetesBoeufs=5;
		else
			nbrTetesBoeufs=1;
		cartes=ajouterCarte(cartes,i,nbrTetesBoeufs,-1);
	}
	return cartes;
}

Carte *melangerCartes(Carte* cartes){

		srand(time(NULL));
	int taille=nbrCartes(cartes);
	Carte *ps=cartes,*psPrecedent=NULL;
	for(int i=0;i<1000;i++){
		int alea=rand()%taille;
		psPrecedent=NULL;
		ps=cartes;
		for(int j=0;j<alea;j++){
			psPrecedent=ps;
			ps=ps->suivant;
		}

		if(psPrecedent){
			psPrecedent->suivant=ps->suivant;
			ps->suivant=cartes;
			cartes=ps;
		}
	}

	return cartes;

}

void initialiserSocketServeur(int *socketServeur,struct sockaddr_in *adresse,const char *adresseIp,const int port){
	*socketServeur=socket(AF_INET,SOCK_STREAM,0);

	if(*socketServeur==0){
		printf("Echec lors de la création su socket serveur\n");
		exit(EXIT_FAILURE);
	}
	//192.168.11.110 /24

	adresse->sin_family=AF_INET;
	adresse->sin_addr.s_addr=inet_addr(adresseIp);// on p
	adresse->sin_port=htons(port);

	if(bind(*socketServeur,(struct sockaddr *)adresse,sizeof(*adresse))<0){
		printf("Erreur lors de l'association du socket avec adresseEtPort\n");
		exit(EXIT_FAILURE);
	}

}

void* demandePseudo(void* socketClient){
	char *pseudo=(char*)malloc(sizeof(char)*25);
	recv(*(int*)socketClient,pseudo,25,0);
	return pseudo;
}
int accepterJoueurs(Serveur *serveur){

		if(listen(serveur->socket,10)<0){
			printf("Erreur lors de l'ecoute\n");
			return -1;
		}
		int socketClients[10];
		struct sockaddr_in adresses[10];
		pthread_t threads[10];
		socklen_t taille=sizeof(struct sockaddr_in);

		time_t  debut,fin;

		time(&debut);
		int dureeAttente=0;
		do{
			printf("En Attente de joueurs %d / 10\n",serveur->nbrJoueurs);
			socketClients[serveur->nbrJoueurs]=accept(serveur->socket,(struct sockaddr*)(adresses+serveur->nbrJoueurs),&taille);

			if(socketClients[serveur->nbrJoueurs]<0){
				printf("Erreur lors de l'acceptation du client\n");
				return -1;
			}
			pthread_create(threads+serveur->nbrJoueurs,NULL,&demandePseudo,socketClients+serveur->nbrJoueurs);
			serveur->nbrJoueurs++;
		  time(&fin);
		  dureeAttente=difftime(fin,debut);	
		}while(!(serveur->nbrJoueurs>=10 || (serveur->nbrJoueurs>=2&&dureeAttente>=30)));
		printf("\n\n\t\t------- Nombre de joueurs : %d  ---------\n\n",serveur->nbrJoueurs);
		for(int i=0;i<serveur->nbrJoueurs;i++){
			char *pseudo=NULL;
			pthread_join(threads[i],(void **)&pseudo);
			serveur->joueurs[i]=creerJoueur(socketClients[i],adresses[i],pseudo);
			afficherJoueur(serveur->joueurs[i]);
			free(pseudo);
		}
			//char pseudo[25];
			//printf("J'attends un signe\n\n");
			//printf("pseudotaille : %d",(int)recv(socketClient,pseudo,25,0));
			//printf("En attentevde \n");
			//recv(socketClient,NULL,sizeof(int),0);
			//serveur->joueurs[serveur->nbrJoueurs]=creerJoueur(socketClient,adresse,pseudo);
			//send(serveur->joueurs[serveur->nbrJoueurs]->socket,serveur->joueurs[serveur->nbrJoueurs],sizeof(Joueur),0);
			//afficherJoueur(serveur->joueurs[serveur->nbrJoueurs]);
			//envoyer(serveur->joueurs[serveur->nbrJoueurs]);
		//Tout est bien passé
		return 0;
}

/*void serialiserCartes(Carte *carte){
	File ficher*=fopen("carte.txt","w");

	while(carte){
		write(carte->num,)
	}
	fclose(ficher);
}*/
void envoyer(Joueur *joueur){
	printf("message1 %d\n",(int)send(joueur->socket,joueur,sizeof(Joueur),0));
	Carte *carte=joueur->cartes;

  int taille=nbrCartes(joueur->cartes);
	printf("message2 %d \n",(int)send(joueur->socket,&taille,sizeof(int),0));

	while(carte){
		printf("message2 %d \n",(int)send(joueur->socket,&carte->num,sizeof(int),0));
		printf("message2 %d \n",(int)send(joueur->socket,&carte->nbrTetesBoeufs,sizeof(int),0));
		printf("message2 %d \n",(int)send(joueur->socket,&carte->joueurIndex,sizeof(int),0));
		carte=carte->suivant;
	}
}

void getConfig(Serveur *serveur){
	FILE *fichier=fopen("jeu.config","r");
	char buffer[400];
	fgets(buffer,400,fichier);
	int i=0;
	while(buffer[i]!=':'){
		serveur->adresseIp[i]=buffer[i];
		i++;
	}
	serveur->adresseIp[i]='\0';
	int x=0;
	while(buffer[++i]!='\n')
		x=x*10+toInt(buffer[i]);

	serveur->port=x;

	fgets(buffer,400,fichier);
	i=0;
	while(buffer[i]!='\n'){
		serveur->description[i]=buffer[i];
		i++;
	}

	serveur->description[i]='\0';

	fclose(fichier);
}
Serveur* creationPartie(){
	Serveur* serveur=(Serveur*)malloc(sizeof(Serveur));
	getConfig(serveur);
	initialiserSocketServeur(&serveur->socket,&serveur->adresse,serveur->adresseIp,serveur->port);

	serveur->joueurs=(Joueur**)malloc(sizeof(Joueur*)*10);
	serveur->nbrJoueurs=0;

		accepterJoueurs(serveur);

	serveur->cartes=creationCartes();
	for(int i=0;i<4;i++)
		serveur->rangees[i]=ajouterCarteRangee(NULL,prendreCarte(&serveur->cartes));

	for(int i=0;i<serveur->nbrJoueurs;i++)
		send(serveur->joueurs[i]->socket,&i,sizeof(int),0);
	
	int p=0;
	for(int i=0;i<serveur->nbrJoueurs;i++)
		recv(serveur->joueurs[i]->socket,&p,sizeof(int),0);

	return serveur;

}

void afficherPartie(Serveur* serveur){

	if(!serveur)
		return ;

	printf("Description:%s\n",serveur->description);

	for(int i=0;i<serveur->nbrJoueurs;i++)
		afficherJoueur(serveur->joueurs[i]);

	afficherRangees(serveur->rangees);
}
void nettoyerServeur(Serveur* serveur){
	if(!serveur)
		return ;
	for(int i=0;i<serveur->nbrJoueurs;i++)
		serveur->joueurs[i]=supprimerJoueur(serveur->joueurs[i]);
	for(int i=0;i<4;i++)
		supprimerCartes(serveur->rangees[i]);
	free(serveur);
}
void supprimerPartie(Serveur *serveur){
	for(int i=0;i<serveur->nbrJoueurs;i++)
		if(serveur->joueurs[i]->socket!=0)
			close(serveur->joueurs[i]->socket);
	nettoyerServeur(serveur);
	if(serveur->socket!=0)
		close(serveur->socket);
}
Carte* rassemblerCarte(Carte *cartes,Carte* carte){
	
	//printf("Dans rassemblerCarte\n");
	if( !cartes || cartes->num>carte->num){
		carte->suivant=cartes;
		return carte;
	}
	Carte* ps=cartes;

	while(ps->suivant && ps->suivant->num<carte->num)
		ps=ps->suivant;
	carte->suivant=ps->suivant;
	ps->suivant=carte;

	return cartes;
}

void tour(Serveur *serveur){

  // à modifier , on doit montrer ça à chacun des joueurs

	for(int i=0;i<serveur->nbrJoueurs;i++){
		afficherRangees(serveur->rangees);
		afficherJoueur(serveur->joueurs[i]);
	}

	printf("Nombre joueur(s) %d \n",serveur->nbrJoueurs);
	Carte* cartesADeposer=NULL;

	pthread_t threads[10];

	printf("En Attente ...\n");
	for(int i=0;i<serveur->nbrJoueurs;i++)
		pthread_create(threads+i,NULL,&jouerCarte,serveur->joueurs[i]);


	for(int i=0;i<serveur->nbrJoueurs;i++){
		Carte *carte=NULL;
		pthread_join(threads[i],(void**)&carte);
		cartesADeposer=rassemblerCarte(cartesADeposer,carte);
	}
	printf("Fin Attente\n");
	while(cartesADeposer){
	//	printf("B1\n");
		Carte *carte=prendreCarte(&cartesADeposer);
	//	printf("B2\n");
		deposerCarteRangee(serveur,carte);
	//	printf("B3\n");
	}
}

int nouveauTour(Serveur *serveur){

	for(int i=0;i<serveur->nbrJoueurs;i++)
		if(serveur->joueurs[i]->score>=66)
			return 0;

	return 1;
}
void distribuerCarte(Serveur *serveur,int joueurIndex){
	Carte* ps=serveur->cartes;
	serveur->cartes=ps->suivant;
	ps->joueurIndex=joueurIndex;
	ps->suivant=serveur->joueurs[joueurIndex]->cartes;
	serveur->joueurs[joueurIndex]->cartes=ps;
}

void ligne(){
	for(int i=1;i<=110;i++){
		if(i%11==0)
			printf("%c",'+');
		else
			printf("%c",'-');
	}
	printf("\n");
}

void debutLigne(){

	for(int i=1;i<=10;i++)
		printf("%9d |",i);
	printf("\n");

}
void* demandeChoisirRangee(void *socketClient){
	int option=2;
	int *indexBonneRangee=(int*)malloc(sizeof(int));
	send(*(int*)socketClient,&option,sizeof(int),0);
	printf("Attente du joueur\n");
	recv(*(int*)socketClient,indexBonneRangee,sizeof(int),0);
	printf("La valeur index est : %d",*indexBonneRangee);
	return indexBonneRangee;
}
void afficherRangees(Carte *rangees[]){

	for(int i=0;i<4;i++){
		printf("rangee %d :\n\t\t",i+1);
		afficherCartes(rangees[i]);
	}
	printf("\n\n");

}
void deposerCarteRangee(Serveur *serveur,Carte *carte){
	printf("Dans depot\n");
	int indexBonneRangee=-1;
	Carte* bonneRangee=NULL;
	for(int i=0;i<4;i++){
		if(serveur->rangees[i]->num < carte->num)
			if(indexBonneRangee==-1 || serveur->rangees[indexBonneRangee]<serveur->rangees[i])
				indexBonneRangee=i;
	}
	if(indexBonneRangee!=-1){
		bonneRangee=serveur->rangees[indexBonneRangee];
		if(nbrCartes(bonneRangee)==5)
		{
			serveur->rangees[indexBonneRangee]=carte;
			serveur->joueurs[carte->joueurIndex]->score+=supprimerCartes(bonneRangee);// Calcul du score et suppression des cartes de la rangée
		}
		else
			serveur->rangees[indexBonneRangee]=ajouterCarteRangee(bonneRangee,carte);
	}
	else
	{

		
		//indexBonneRangee=choisirRangee(serveur->rangees);
		pthread_t thread;
		pthread_create(&thread,NULL,&demandeChoisirRangee,&(serveur->joueurs[carte->joueurIndex]->socket));
		/*printf("Avant send\n");
		send(serveur->joueurs[carte->joueurIndex]->socket,&option,sizeof(int),0);
		printf("En attente de l'index\n");
		recv(serveur->joueurs[carte->joueurIndex]->socket,&indexBonneRangee,sizeof(int),0);
		*/
		int *resultat=NULL;
		pthread_join(thread,(void**)&resultat);
		indexBonneRangee=*resultat;

		printf("bonne rangee %d \n",indexBonneRangee);
		bonneRangee=serveur->rangees[indexBonneRangee];
		serveur->rangees[indexBonneRangee]=carte;
		serveur->joueurs[carte->joueurIndex]->score=supprimerCartes(bonneRangee);//Calcul du score et suppression des cartes de la rangée
	}
}
int manche(Serveur *serveur){
	//printf("AA\n");
	serveur->cartes=melangerCartes(serveur->cartes);
	//printf("AB\n");
	for(int i=0;i<10;i++)
		for(int indexJoueur=0;indexJoueur<serveur->nbrJoueurs;indexJoueur++)
			distribuerCarte(serveur,indexJoueur);
	//printf("AC\n");
	int option=3;
	pthread_t threads[10];

	for(int nbrTour=1;nbrTour<=10;nbrTour++){
		misAjourServeur(serveur);
		afficherScores(serveur);
		for(int i=0;i<serveur->nbrJoueurs;i++)
			pthread_create(threads+i,NULL,&demandeAfficherScores,&serveur->joueurs[i]->socket);
		for (int i = 0; i < serveur->nbrJoueurs; i++)
			pthread_join(threads[i],NULL);	

		tour(serveur);
		if(nouveauTour(serveur))
			printf("\n\nNouveau tour\n\n");
		else
				return 0;
	}
	printf("AD\n");
	return 1;

}

void* demandeAfficherScores(void *socketClient){
	int option=4,ack=0;
	send(*(int*)socketClient,&option,sizeof(int),0);
	recv(*(int*)socketClient,&ack,sizeof(int),0);
}

void clearScreen(){
	printf("\033[2J\033[H");
}
void afficherScores(Serveur *serveur){

	int arr[10];
	for(int i=0;i<10;i++)
		arr[i]=i;
	for(int i=0;i<serveur->nbrJoueurs-1;i++)
	{
		for(int j=i+1;j<serveur->nbrJoueurs;j++){
			if(serveur->joueurs[arr[i]]->score > serveur->joueurs[arr[j]]->score){
				int tmp=arr[i];
				arr[i]=arr[j];
				arr[j]=tmp;
			}
		}
	}
	sleep(2);
	clearScreen();
	printf("\n\n\t\t -------- Nombre de joueurs :  %d ---------\n",serveur->nbrJoueurs);

	printf("------ Classement -------\n");
	for(int i=0;i<serveur->nbrJoueurs;i++)
		printf("%d - %s a %d tetes de boeufs\n",i+1,serveur->joueurs[arr[i]]->pseudo,serveur->joueurs[i]->score);
	printf("\n\n\n");
}

void serialiserCarte(FILE *fic,Carte *cartes){
    if(!fic)
        return ;
    while(cartes){
        fprintf(fic,"%d,%d,%d;",cartes->num,cartes->nbrTetesBoeufs,cartes->joueurIndex);
        cartes=cartes->suivant;
    }
    fprintf(fic,":");
}

int toInt(char c){
    return c-'0';
}

 void deserialiserCarte(char *buffer,Carte **rangees,int rangee){
    int i=0;
    int num=-1,nbrTetesBoeufs=-1,indexJoueur=-1;
    int signe=1;
    int nbrCartes=0;
    int indexRangee=0,x=0;
    Carte *cartes=NULL;
    while(buffer[i]!='\n')
    {
        if(buffer[i]==':'){
            rangees[indexRangee++]=cartes;
            if(rangee==1)
                return ;
            else
                cartes=NULL;
        }
        else if(buffer[i]==','){
            if(num==-1)
                num=x*signe;
            else 
                nbrTetesBoeufs=x*signe;

            x=0;
            signe=1;
        }
        else if(buffer[i]==';'){
            indexJoueur=x*signe;
            cartes=ajouterCarte(cartes,num,nbrTetesBoeufs,indexJoueur);
            x=0;
            signe=1;
            num=-1;
        }
        else if(buffer[i]=='-')
            signe=-1;
        else
            x=x*10+toInt(buffer[i]);
        
        i++;
    }
}

int serialiserServeur(const Serveur *serveur){
    FILE * fichier=fopen("serveurUpdated.jeu","w");
    fprintf(fichier, "%s,%d,%d:",serveur->description, serveur->nbrJoueurs,serveur->socket);
    for (int i = 0; i < 4; i++){
        serialiserCarte(fichier,serveur->rangees[i]);
    }

    fprintf(fichier,"\n");

    for(int i=0;i<serveur->nbrJoueurs;i++){
        fprintf(fichier,"%s,%d,%d:",serveur->joueurs[i]->pseudo,serveur->joueurs[i]->score,serveur->joueurs[i]->socket);
        serialiserCarte(fichier,serveur->joueurs[i]->cartes);
        fprintf(fichier,"\n");
    }
    fclose(fichier);
    return 0;
}



// consiste à deserialiser le fichier dans lequel le serveur met ses modifs
Serveur* deserialiserServeur(const char *chemin)
{
    FILE *fichier=fopen(chemin,"r");
    char buffer[400];
    int nbrLigne=0,i=0,x=0;
    if(!fichier){
        printf("Fichier introuvable !!!\n");
        exit(EXIT_FAILURE);
    }

    Serveur * serveur=(Serveur*) malloc(sizeof(Serveur));
    if(fgets(buffer,400,fichier)){
        while(buffer[i]!=','){
            serveur->description[i]=buffer[i];
            i++;
        }
        serveur->description[i]='\0';
        
        x=0;
        while(buffer[++i]!=',')
            x=x*10+toInt(buffer[i]);
        serveur->nbrJoueurs=x;

        x=0;
        while(buffer[++i]!=':')
            x=x*10+toInt(buffer[i]);
        serveur->socket=x;
        serveur->joueurs=(Joueur**)malloc(sizeof(Joueur)*serveur->nbrJoueurs);

        // Le client n'a pas besoin de connaitre cette info
        serveur->cartes=NULL;

        deserialiserCarte(buffer+i+1,serveur->rangees,4);
        nbrLigne++;
    }
    else
        return NULL;

    while(fgets(buffer,400,fichier)){
        i=0;
        x=0;
        serveur->joueurs[nbrLigne-1]=(Joueur*)malloc(sizeof(Joueur));
        while(buffer[i]!=','){
            serveur->joueurs[nbrLigne-1]->pseudo[i]=buffer[i];
            i++;
        }
        
        serveur->joueurs[nbrLigne-1]->pseudo[i]='\0';

        while(buffer[++i]!=',')
            x=x*10+toInt(buffer[i]);
        serveur->joueurs[nbrLigne-1]->score=x;

        x=0;
        while(buffer[++i]!=':')
            x=x*10+toInt(buffer[i]);
        serveur->joueurs[nbrLigne-1]->socket=x;

        deserialiserCarte(buffer+i+1,&serveur->joueurs[nbrLigne-1]->cartes,1);

        nbrLigne++;
    }

    fclose(fichier);

    if(nbrLigne-1!=serveur->nbrJoueurs)
    {
        perror("Erreur lors de la synchronisation\n");
        return NULL;
    }
    return serveur;

}

void misAjourServeur(Serveur *serveur){

    serialiserServeur(serveur);
    FILE *fichier=fopen("serveurUpdated.jeu","r");
    char buffer[400];
    //printf("nbr joeus :%d",serveur->nbrJoueurs);
    int p=3;
    for(int i=0;i<serveur->nbrJoueurs;i++)
        		send(serveur->joueurs[i]->socket,&p,sizeof(int),0);

    while(fgets(buffer,400,fichier)){
    	//printf("message envoyé : %s",buffer);
        for(int i=0;i<serveur->nbrJoueurs;i++)
        		send(serveur->joueurs[i]->socket,buffer,400,0);

           // printf("taille : %d\n",(int) send(serveur->joueurs[i]->socket,buffer,400,0));
    }

    for(int i=0;i<serveur->nbrJoueurs;i++)
        		recv(serveur->joueurs[i]->socket,&p,sizeof(int),0);

    fclose(fichier);
}


/*int main(int argc,char *argv[]){

	Serveur *serveur=creationPartie();
	serveur->cartes=melangerCartes(serveur->cartes);
	while(manche(serveur));
	afficherScores(serveur);
	supprimerPartie(serveur);
}*/