
#include <stdio.h>
#include<stdlib.h>
#include <unistd.h>
#include "inclusions.h"
#include "Carte.h"


uint8_t ChooseCard(Ligne* rows, Carte* hand, int mode);
uint8_t ChooseRow(Ligne* rows, Carte* hand, int mode);

// Stratégie : on choisit la carte de plus haute valeur
uint8_t ChooseCard_MaxVal(Ligne* rows, Carte* hand);
// Stratégie : on choisit la carte de plus basse valeur
uint8_t ChooseCard_MinVal(Ligne* rows, Carte* hand);
// Stratégie : on choisit une carte au hasard
uint8_t ChooseCard_Random(Ligne* rows, Carte* hand);
// Stratégie : on choisit la colonne de plus faible valeur de vaches cumulées
uint8_t ChooseRow_MinCowsSum(Ligne* rows, Carte* hand);
// Stratégie : on choisit une colonne au hasard
uint8_t ChooseRow_Random(Ligne* rows, Carte* hand);

