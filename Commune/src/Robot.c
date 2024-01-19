#include "Robot.h"

#include <stdio.h>
#include <stdlib.h>
#include "Carte.h"
#include "Joueurs.h"

uint8_t ChooseCard(Ligne* rows, Carte* hand, int mode)
{
    switch(mode)
    {
    case 1:
        return ChooseCard_MaxVal(rows, hand);
    case 2:
        return ChooseCard_MinVal(rows, hand);
    case 3:
        return ChooseCard_Random(rows, hand);
    default:
        fprintf(stderr, "Mode de choix de carte inexistant.\n");
        exit(EXIT_FAILURE);
    }
}

uint8_t ChooseRow(Ligne* rows, Carte* hand, int mode)
{
    switch(mode)
    {
    case 1:
        return ChooseRow_MinCowsSum(rows, hand);
    case 2:
        return ChooseRow_Random(rows, hand);
    default:
        fprintf(stderr, "Mode de choix de colonne inexistant.\n");
        exit(EXIT_FAILURE);
    }
}
uint8_t ChooseCard_MaxVal(Ligne* rows, Carte* hand)
{
    uint8_t selected = 0;

    for (size_t i = 1; i < 10; i++)
    {
        if (hand[i].numero > 0 && hand[i].tete_boeuf > 0 &&
            (hand[i].numero > hand[selected].numero || hand[selected].numero == 0))
        {
            selected = i;
        }
    }

    return selected +1;
}

uint8_t ChooseCard_MinVal(Ligne* rows, Carte* hand)
{
    uint8_t selected = 0;

    for (size_t i = 1; i < 10; i++)
    {
        if (hand[i].numero > 0 && hand[i].tete_boeuf > 0 &&
            (hand[i].numero < hand[selected].numero || hand[selected].numero == 0))
        {
            selected = i;
        }
    }
    

    return selected +1;
}

uint8_t ChooseCard_Random(Ligne* rows, Carte* hand)
{
    uint8_t selected = 0;
    size_t availableCards = 0;

    for (size_t i = 1; i < 10; i++)
    {
        if (hand[i].numero > 0 && hand[i].tete_boeuf > 0)
            availableCards++;
    }

    if (availableCards > 0)
    {
        uint8_t randVal = rand() % availableCards + 1;

        // On va itérer entre 1 et 10 fois sur les cartes de la main et sélectionner la carte
        for (size_t i = 1; randVal > 0; i = (i + 1) % 10)
        {
            if (hand[i].numero > 0 && hand[i].tete_boeuf > 0)
                randVal--;

            if (randVal == 0)
                selected = i;
        }
    }

    return selected +1;
}
uint8_t ChooseRow_MinCowsSum(Ligne* rows, Carte* hand)
{
    uint8_t out = 0;
    uint8_t outCowsSum = 0;

    for(size_t i = 0 ; i < NOMBRE_LIGNE ; i++)
    {
        uint8_t cowsSum = 0;

        for(size_t j = 0 ; j < rows[i].taille ; j++)
        {
            cowsSum += rows[i].Cartes[j].tete_boeuf;
        }

        if(cowsSum < outCowsSum)
        {
            out = i;
            outCowsSum = cowsSum;
        }
    }

    return out +1; 
}

uint8_t ChooseRow_Random(Ligne* rows, Carte* hand)
{
    return rand() % NOMBRE_LIGNE +1; 
}