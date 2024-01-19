
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "Client.h"
#include "Carte.h"
#define MAX_CARTES 10
typedef struct Joueur
{
    char nom[25];
    Carte main[MAX_CARTES];
    int score;
    Carte CarteChoisie;
    int IndiceCarteChoisie;
    char nomGroupe[1024];
    Client c;
    bool isInChat;
    bool hasEnteredChat;
}Joueur;

typedef struct ListeJoueur
{
    Joueur *listeJoueurs; 
    size_t size;
    
}ListeJoueur;

//Quand le joueur doit prendre toutes les cartes de la rangée
void PrendTouteLigne(Joueur* joueur, Ligne* ligne);

Joueur* JoueurCarteMin(const ListeJoueur joueurs);

int CheckEndGame(const ListeJoueur joueurs, ListeJoueur robots);