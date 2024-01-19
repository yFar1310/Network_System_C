#include "Joueurs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int CheckEndGame(const ListeJoueur joueurs, ListeJoueur robots)
{
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        if(joueurs.listeJoueurs[i].score <= 0) return 1;
    }
    for(size_t i = 0 ; i < robots.size ; i++)
    {
        if(robots.listeJoueurs[i].score <= 0) return 1;
    }

    return 0;
}

void PrendTouteLigne(Joueur* joueur, Ligne* ligne)
{
    for(size_t i = 0 ; i < ligne->taille ; i++)
    {
        joueur->score -= ligne->Cartes[i].tete_boeuf;
        ligne->Cartes[i].numero = 0; // On supprime la carte de la colonne
        ligne->Cartes[i].tete_boeuf = 0;
    }

    ligne->taille = 0; // La colonne est maintenant vide.

}

Joueur* JoueurCarteMin(const ListeJoueur joueurs)
{
    Joueur* out = NULL;
    
    for(size_t i = 0 ; i < joueurs.size ; i++)
    {
        Joueur* J = &joueurs.listeJoueurs[i];

        // Si le joueur courant n'a choisi aucune carte, alors on passe au joueur suivant.
        if(J->CarteChoisie.numero == 0) continue;

        // Sinon : si "out" n'a pas encore été défini,
        // ou alors, si "out" est défini mais sa carte choisie a une plus grande valeur que la carte choisie du joueur courant,
        // alors "out" devient le joueur courant.
        if(out == NULL || J->CarteChoisie.numero < out->CarteChoisie.numero)
            out = J;
    }

    return out;

}


