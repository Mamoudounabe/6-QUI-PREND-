#include<stdio.h>
#include<stdlib.h>
#include"serveur.h"



int main(void){
    Serveur *serveur=creationPartie(); 
    while(manche(serveur));
    
    for(int i=0,option=4;i<serveur->nbrJoueurs;i++)
        send(serveur->joueurs[i]->socket,&option,sizeof(int),0);

    for(int i=0,option=0;i<serveur->nbrJoueurs;i++)
        recv(serveur->joueurs[i]->socket,&option,sizeof(int),0);

    supprimerPartie(serveur);
   return 0;
}