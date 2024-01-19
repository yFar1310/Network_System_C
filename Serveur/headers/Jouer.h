#include "Joueurs.h"
#include "Carte.h"
#include "string.h"
#include "serveur.h"
#include "Robot.h"
void DemanderNom(ListeJoueur lj,ListeJoueur rbt);
void DemanderEquipe(ListeJoueur lj);
void startGame(ListeClients *lc,fd_set *rdfs,int nbRobots);
void distruCartes(ListeJoueur lj,Paquet *p);
int Manche(fd_set *rdfs);
void Tour(ListeJoueur lj,ListeJoueur robots, fd_set *rdfs);
void send_message_to_Equipe(ListeJoueur lj,Client sender,char* buffer);
void ExitChat(ListeJoueur *lj, int playerIndex);
void envoyerMainAuClient(ListeJoueur lj,int j);
void EnvoyerMainAuGroupe(ListeJoueur lj, int joueurActuel);
void AjouteCarteTable(ListeJoueur lj);
void ajouterCarteRobot();
void DemanderCarte(ListeJoueur lj);
void DemandercarteRobot();
void AfficheInfosJoueurs(ListeJoueur lj, fd_set* rdfs);
void creerFichierEtRepertoire(FILE *fichier);