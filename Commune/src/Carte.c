#include "Carte.h"
#include "time.h"
#include <stdio.h>
/*Fonctions relative au paquet*/

void Construction_Paquet(Paquet *paquet)
{
    Carte c;
    for(int i=0;i<NOMBRE_CARTE;i++)
    {
        size_t nb = i + 1;
        paquet->Cartes[i].numero = nb;

         paquet->Cartes[i].tete_boeuf = 0;

        if(nb % 5 == 0)  paquet->Cartes[i].tete_boeuf += 2;
        if(nb % 10 == 0)  paquet->Cartes[i].tete_boeuf += 1;
        if(nb % 11 == 0)  paquet->Cartes[i].tete_boeuf += 5;

        if( paquet->Cartes[i].tete_boeuf == 0)  paquet->Cartes[i].tete_boeuf = 1;

        paquet->curseur++;
        switch(paquet->Cartes[i].tete_boeuf)
        {
            case 1:
                 paquet->Cartes[i].Couleur = 35;
                 break;
            case 2:
                 paquet->Cartes[i].Couleur = 32;
                 break;
            case 3:
                 paquet->Cartes[i].Couleur = 93;
                 break;
            case 5:
                 paquet->Cartes[i].Couleur = 32;
                 break;
            case 7:
                 paquet->Cartes[i].Couleur= 31;
                 break;
                 
        }
    }
    paquet->curseur = NOMBRE_CARTE;
    Melange_Paquet(paquet);

}

void Melange_Paquet(Paquet *paquet)
{
    srand(time(NULL));
    int r;
    Carte c;
    for(int i=0;i<paquet->curseur;i++)
    {
        r = rand() % NOMBRE_CARTE;
        c=paquet->Cartes[i];
        paquet->Cartes[i]=paquet->Cartes[r];
        paquet->Cartes[r]=c;
    }
}

void Affiche_Paquet(Paquet *paquet)
{
    printf("position dans le paquet | Numero | Tete de boeuf\n");

    for(size_t i = 0 ; i < NOMBRE_CARTE ; i++)
    {
        printf("%7lu        %3u     %2u\n", i, paquet->Cartes[i].numero, paquet->Cartes[i].tete_boeuf);
    }
}


Carte Tire_Carte(Paquet *paquet)
{
    return paquet->Cartes[--paquet->curseur];
}

/*Fonctions relatives aux rangées*/

void Construction_Ligne(Ligne *ligne)
{
    Efface_Ligne(ligne);

}

void Pose_Carte_Ligne(Ligne *ligne,Carte carte)
{
    if(ligne->taille < 5)
    {
        ligne->Cartes[ligne->taille] = carte;
        ligne->taille ++;
    }
    else
    {
        fprintf(stderr, "Impossible d'ajouter une sixieme carte dans la rangee.\n");
        exit(EXIT_FAILURE);
    }

}

void Efface_Ligne(Ligne* ligne)
{
    ligne->taille = 0;
}

Ligne* getBonneLigne(Ligne* lignes, Carte carte)
{
    Ligne* bonneLigne = NULL;
 
    for(int i = 0 ; i < NOMBRE_LIGNE ; i++)
    {
     
        Ligne* ligne = lignes + i;
        int DerniereValeur= ligne->Cartes[ligne->taille - 1].numero;
        
        if(ligne->taille > 0 && carte.numero < DerniereValeur)continue;

        if(bonneLigne == NULL || DerniereValeur > bonneLigne->Cartes[bonneLigne->taille - 1].numero)
            bonneLigne = ligne;
    }

    return bonneLigne;

}



