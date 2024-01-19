#include "Jouer.h"
#include <ctype.h>


// Variables statiques (= limitées à cette unité de traduction)
static ListeJoueur joueurs;
static ListeJoueur robots;
static  Paquet p = { 0 };
static  Ligne Lignes[NOMBRE_LIGNE] = { 0 };
static size_t Nombre_Tour = 0, Nombre_Manche = 0;
static int nbA;
static int nbB;
static FILE* fichier;
static FILE* histoChatA;
static FILE* histoChatB;

void DemanderNom(ListeJoueur lj,ListeJoueur rbt)
{
    char* phrase = "Veuillez entrez votre nom";
    for (int i = 0; i < lj.size; i++)
    {
        char buffer[1024];
        
        write_client(lj.listeJoueurs[i].c.sock,phrase);
        read_client(lj.listeJoueurs[i].c.sock, buffer);
        strcpy(lj.listeJoueurs[i].c.name, buffer);
        strcpy(lj.listeJoueurs[i].nom, buffer);
         // Initialiser isInChat à true lorsque vous créez un nouveau joueur
        lj.listeJoueurs[i].isInChat = true;
        lj.listeJoueurs[i].hasEnteredChat = false;
        printf("JOUEUR numero %d , %s\n",i, lj.listeJoueurs[i].c.name);
        
    }
    for(int j=0;j<rbt.size;j++)
    {   
            char name[20];
            snprintf(name, sizeof(name), "Robot_%d", j + 1);            
            strcpy(rbt.listeJoueurs[j].nom,name);          
            printf("NOM ROBOT  %s\n",rbt.listeJoueurs[j].nom);
    }
}

void DemanderEquipe(ListeJoueur lj)
{
    char* phrase = "Veuillez choisir un groupe : [A,B]";
    for (int i = 0; i < lj.size; i++)
    {
        char buffer2[1024];
        int validChoice = 0;

        while (!validChoice) {
            
                write_client(lj.listeJoueurs[i].c.sock, phrase);
                int bytesRead = read_client(lj.listeJoueurs[i].c.sock, buffer2);
                if (bytesRead <= 0)
                {
                    // Gérer la déconnexion du client ou l'erreur lors de la lecture du groupe
                    printf("Client disconnected or error while reading group.\n");
                    // Éventuellement, supprimez le joueur de la liste ou prenez les mesures appropriées.
                    break;  // Sortir de la boucle en cas de déconnexion ou d'erreur
                }

                if (strlen(buffer2) == 1 && (buffer2[0] == 'A' || buffer2[0] == 'B'))
                {
                    printf("JOUEUR a choisi le groupe: %c\n", buffer2[0]);
                    strcpy(lj.listeJoueurs[i].nomGroupe, buffer2);
                    lj.listeJoueurs[i].c.idEquipe = (buffer2[0] == 'A') ? 1 : 2;
                    nbA += (buffer2[0] == 'A') ? 1 : 0;
                    nbB += (buffer2[0] == 'B') ? 1 : 0;
                    validChoice = 1;
                }
                else
                {
                    char* msgerror = "Choix invalide. veuillez choisir entre 'A' et 'B'.\n";
                    write_client(lj.listeJoueurs[i].c.sock, msgerror);
                }
            
        }
    }
    for(int j=0;j<robots.size;j++)
    {
         // Choix aléatoire pour le robot ('A' ou 'B')
                char randomChoice = (rand() % 2) == 0 ? 'A' : 'B';
                printf("ROBOT a choisi le groupe: %c\n", randomChoice);
                strcpy(robots.listeJoueurs[j].nomGroupe, &randomChoice);
                robots.listeJoueurs[j].c.idEquipe = (randomChoice == 'A') ? 1 : 2;
                nbA += (randomChoice == 'A') ? 1 : 0;
                nbB += (randomChoice == 'B') ? 1 : 0;
    }
}

void distruCartes(ListeJoueur lj,Paquet *p)
{
   for(int i=0 ; i<lj.size;i++)
   {
    printf("MAIN DU JOUEUR : %s \n",lj.listeJoueurs[i].nom);
    for(int j=0;j<10;j++)
    {
    lj.listeJoueurs[i].main[j] = Tire_Carte(p);
     printf("  %d [ \033[%um%3u  |  %-1u\033[0m ]    \n", j,lj.listeJoueurs[i].main[j].Couleur, lj.listeJoueurs[i].main[j].numero,lj.listeJoueurs[i].main[j].tete_boeuf);
    }
   }

    // Distribuer des cartes aux robots
    for (int i = 0; i < robots.size; i++)
    {
        printf("CARTE DU ROBOT : %s \n", robots.listeJoueurs[i].nom);
        for (int j = 0; j < 10; j++)
        {
            robots.listeJoueurs[i].main[j] = Tire_Carte(p);
            printf("  %d [ \033[%um%3u  |  %-1u\033[0m ]    \n", j, robots.listeJoueurs[i].main[j].Couleur, robots.listeJoueurs[i].main[j].numero, robots.listeJoueurs[i].main[j].tete_boeuf);
        }
    }
}

void envoyerMainAuClient(ListeJoueur lj,int j) {
    // Construire une chaîne de caractères représentant la main
    char message[1024];
    message[0] = '\0';
    char carteStr[1024];
    

     // En-tête
    sprintf(carteStr, "\n%-8s%-8s%-8s\n", "Indice", "Numero", "Boeuf");
    strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

    for (int i = 0; i < 10; i++) {
        if (lj.listeJoueurs[j].main[i].numero == 0 && lj.listeJoueurs[j].main[i].tete_boeuf == 0) {
            // Carte jouée, ajoutez le texte approprié
            sprintf(carteStr, "  %2d [\033[%umCarteJouée\033[0m]    \n", i + 1, lj.listeJoueurs[j].main[i].Couleur);
        } else {
            sprintf(carteStr, "  %2d [ \033[%um%3u  |  %-1u\033[0m ] \n", i + 1, lj.listeJoueurs[j].main[i].Couleur, lj.listeJoueurs[j].main[i].numero, lj.listeJoueurs[j].main[i].tete_boeuf);
        }

        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
    }

    // Envoyer la main au client
    message[1024]=0;
    carteStr[1024]=0;
    write_client(lj.listeJoueurs[j].c.sock, message);
    memset(carteStr, 0, sizeof(carteStr));
    memset(message,0,sizeof(message));
}

void envoyerTableJeu(Ligne *rows,ListeJoueur lj,int j)
{
     // Construire une chaîne de caractères représentant la table de jeu
    char message[1024];
    message[0] = '\0';
    char carteStr[200];
    // Construire une chaîne de caractères représentant la main sans codes de couleur pour le fichier
    char chatStr[1024];
    chatStr[0] = '\0';
    char carteFileStr[200];
    
    fichier = fopen("Historiques","a");
    
    sprintf(carteStr,"\n\n\t\t\t\t#############  TABLE DU JEU  ############# \n\n");
    strncat(message,carteStr,sizeof(message) - strlen(message) - 1);

    snprintf(carteFileStr, sizeof(carteFileStr), "\n#############  TABLE DU JEU  ############# \n\n");
    strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);

    
    for (size_t i = 0; i < 4; i++) // On regarde dans les 4 colonnes
    {
        sprintf(carteStr, "\t\t\t\t[ ");
        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

        sprintf(carteFileStr, "[ ");
        strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);

        for (size_t k = 0; k < rows[i].taille; k++) // On a max 5 cartes par colonne (donc 5 lignes max)
        {
            sprintf(carteStr, "\033[%um%3u  |  %-1u\033[0m",rows[i].Cartes[k].Couleur, rows[i].Cartes[k].numero, rows[i].Cartes[k].tete_boeuf);
            strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

            sprintf(carteFileStr,  " [ %3u  |  %-1u ] ", rows[i].Cartes[k].numero, rows[i].Cartes[k].tete_boeuf);
            strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);

            if (k < rows[i].taille - 1)
            {
                sprintf(carteStr, " , ");
                strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

                sprintf(carteFileStr, " , ");
                strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);
            }
        }

        sprintf(carteStr, " ] \n");
        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

        sprintf(carteFileStr, " ]");
        strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);

        if (i < 3)
        {
            sprintf(carteStr, "\t\n");
            strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

            sprintf(carteFileStr, "\n");
            strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);
        }
    }   
    // Envoyer la table de jeu au client
    write_client(lj.listeJoueurs[j].c.sock, message);
    
    fprintf(fichier, "%s\n", chatStr);

    fclose(fichier);
}
//FONCTION POUR LA COMMUNICATION ENTRE LES MEMBRES DE L'ÉQUIPE
int a = 1;
int countA=0;
int countB=0;
void Chat(ListeJoueur lj,fd_set *rdfs)
{ 
    a=1;
    while (a) {
        usleep(100); // Pause de 1000 milliseconde
        for (int i = 0; i < lj.size; i++) {
            //sleep(1);
            if (!lj.listeJoueurs[i].hasEnteredChat && lj.size>1 && lj.listeJoueurs[i].c.idEquipe==1) {
                write_client(lj.listeJoueurs[i].c.sock, "\n\t\t#############  BIENVENUE AU CHAT A ############# \n");
                lj.listeJoueurs[i].hasEnteredChat = true;  // Marquer comme entré dans le chat
            }else if(!lj.listeJoueurs[i].hasEnteredChat && lj.size>1 && lj.listeJoueurs[i].c.idEquipe==2)
            {
                 write_client(lj.listeJoueurs[i].c.sock, "\n\t\t#############  BIENVENUE AU CHAT B ############# \n");
                lj.listeJoueurs[i].hasEnteredChat = true;  // Marquer comme entré dans le chat
            }
            char buffer[1024];
            fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, SOCK_NONBLOCK);
            if(FD_ISSET(lj.listeJoueurs[i].c.sock,rdfs) && lj.listeJoueurs[i].isInChat)
            {
               //printf("avant read_C\n");
                // On utilise recv avec l'option MSG_DONTWAIT pour rendre la lecture non bloquante
               int n = read_client(lj.listeJoueurs[i].c.sock,buffer);
               //printf("apres read_C\n");
               if(strlen(buffer)>0){
                printf("JOUEUR %s a dit : %s\n", lj.listeJoueurs[i].nom,buffer);
                
                 // Vérifier si le joueur veut quitter le chat
                    if (strcmp(buffer, "exitChat") == 0)
                    {
                        ExitChat(&lj, i);
                        //a=0;
                        memset(buffer, 0, sizeof(buffer));
                    }
                    else if (strcmp(buffer, "sendHand") == 0)
                    {
                        // Envoyer la main du joueur aux autres membres du groupe
                        EnvoyerMainAuGroupe(lj, i);
                        memset(buffer, 0, sizeof(buffer));
                    }
                    else
                    {
                    if(lj.listeJoueurs[i].c.idEquipe == 1)
                    {

                      
                       send_message_to_Equipe(lj,lj.listeJoueurs[i].c,buffer);
                       memset(buffer, 0, sizeof(buffer));
                    }
                    else{
                         
                         send_message_to_Equipe(lj,lj.listeJoueurs[i].c,buffer);
                         memset(buffer, 0, sizeof(buffer));
                        }
                    }
               }
               if(countA==nbA  && countB==nbB)
            {
                printf("on a quitter le chat\n");
                a=0;

            }
            }            
            
        }
        
   }
   countA = 0;
   countB = 0;
}

void update_fd_set(ListeJoueur lj, fd_set *rdfs) {
    FD_ZERO(rdfs);
    for (int i = 0; i < lj.size; i++) {
        FD_SET(lj.listeJoueurs[i].c.sock, rdfs);
    }
}
int openChat = 0;
    int nbOpenChatA = 0;
    int nbOpenChatB = 0;
    int avoidA = 0;
    int avoidB = 0;
void AfficheInfosJoueurs(ListeJoueur lj, fd_set* rdfs)
{
    fichier = fopen("Historiques", "a");
    fprintf(fichier, "Scores et information des joueurs :\n");
    
    printf("\t\tScores et informations des joueurs :\n");
     
    char message[1024];
    for(int i = 0; i < lj.size; i++)
    {
        char msg[200];
        char Stat[200];
        // Utilisez sprintf pour formater le message
        sprintf(msg, "\t\tJoueur %.50s (Groupe %.50s, Score %d)\n", lj.listeJoueurs[i].nom, lj.listeJoueurs[i].nomGroupe, lj.listeJoueurs[i].score);
        sprintf(Stat, "Joueur %.50s (Groupe %.50s, Score %d)\n", lj.listeJoueurs[i].nom, lj.listeJoueurs[i].nomGroupe, lj.listeJoueurs[i].score);
        strncat(message,msg,sizeof(message) - strlen(message) - 1);
       

        //ajout au fichier historiques
        fprintf(fichier, "%s", Stat);
        // Imprimez le message côté serveur 
        printf("%s", msg);
    }
     for (int i = 0; i < robots.size; i++)
    {
        char msg[2000];
        char Stat[2000];
        // Utilisez sprintf pour formater le message des robots
        sprintf(msg, "\t\tRobot %.50s (Groupe %s, Score %d)\n", robots.listeJoueurs[i].nom,robots.listeJoueurs[i].nomGroupe, robots.listeJoueurs[i].score);
        sprintf(Stat, "Robot %.50s (Groupe %s, Score %d)\n", robots.listeJoueurs[i].nom,robots.listeJoueurs[i].nomGroupe, robots.listeJoueurs[i].score);
        // Utilisez strncat si vous souhaitez ajouter d'autres informations au message
        strncat(message, msg, sizeof(message) - strlen(message) - 1);

        // Imprimez le message des robots côté serveur
        printf("%s", msg);
        fprintf(fichier, "%s", Stat);
    }
    
    for(int i=0;i<lj.size;i++)
    {

        // Utilisez write_client pour envoyer le message au joueur
        write_client(lj.listeJoueurs[i].c.sock, message);
    }

    printf("\n");
    fclose(fichier);
}

void Tour(ListeJoueur lj,ListeJoueur rbt, fd_set *rdfs)
{    
    Nombre_Tour++;
    fichier = fopen("Historiques", "a");
    fprintf(fichier, "DEBUT TOUR NUMERO : %ld \n", Nombre_Tour);
    fclose(fichier);
    
    AfficheInfosJoueurs(lj,rdfs);
    char* phrase = "'open' pour ouvrir le chat 'skip' pour ignorer\n";
    for (int i = 0; i < lj.size; i++)
    {
        envoyerTableJeu(Lignes,lj,i);
        usleep(20);
        envoyerMainAuClient(lj,i);
    }
    for (int i = 0; i < lj.size; i++)
    {
         int flags = fcntl(lj.listeJoueurs[i].c.sock, F_GETFL, 0);
             flags &= ~O_NONBLOCK;  // Supprimer le drapeau O_NONBLOCK
           fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, flags);

        char buffer[1024];
        int playerChoice = 0; // 0: No choice, 1: 'open', 2: 'skip'
        
        while (!playerChoice && (nbA>1 || nbB>1))
        {
            //printf("sa\n");
            
             
                write_client(lj.listeJoueurs[i].c.sock, phrase);
            
            read_client(lj.listeJoueurs[i].c.sock, buffer);
            buffer[1024]=0;
            printf("%s  \n",buffer);

            if (strcmp(buffer, "open") == 0 || strcmp(buffer, "skip") == 0)
            {
                // La décision est correcte, on peut sortir de la boucle
                playerChoice = 1;

                // Si le joueur choisit 'openChat', incrémente le nombre d'ouvertures du chat
                if (strcmp(buffer, "open") == 0)
                {
                    memset(buffer, 0, sizeof(buffer));
                    printf("before\n");
                    
                    lj.listeJoueurs[i].isInChat = true;
                    printf("after\n");
                    if(lj.listeJoueurs[i].c.idEquipe==1)
                    {
                        nbOpenChatA++;
                        printf("nbOpenChatA (open) %d\n",nbOpenChatA);

                    }
                    else nbOpenChatB++;
                
                }
                if(strcmp(buffer , "skip") == 0)
                {
                    printf("in skip\n");
                    memset(buffer, 0, sizeof(buffer));
                    
                
                    if(lj.listeJoueurs[i].c.idEquipe==1)
                    {   
                        avoidA = 1;
                        nbOpenChatA--;
                        printf("nbOpenChatA %d  \n",nbOpenChatA);
                    }
                    else{
                        nbOpenChatB--;
                        avoidB = 1;
                    }
                }
                  //playerChoice = 1;
                
            }
        }       
        //openChat=0;     
    }    
    if((nbOpenChatA == nbA || nbOpenChatB == nbB) && (avoidA==0 && avoidB==0))
    {
        
        usleep(20);
        Chat(lj,rdfs);
        nbOpenChatA =0;
        nbOpenChatB = 0;
        avoidA = 0;
        avoidB = 0;
        for(size_t i =0;i<lj.size;i++)
        {
            lj.listeJoueurs[i].hasEnteredChat = false;
        } 
    }
     if(nbOpenChatA<=0 && nbOpenChatB<=0 || (avoidA==1 && avoidB==1))
     {
        for(size_t i=0;i<lj.size;i++)
        {
        lj.listeJoueurs[i].isInChat = false;
        }
       nbOpenChatA =0;
        nbOpenChatB = 0;
        avoidA = 0;
        avoidB = 0;
        DemanderCarte(lj);
        DemandercarteRobot();
        //update_fd_set(lj,rdfs);
        AjouteCarteTable(lj);
        ajouterCarteRobot();
     }    
     fichier = fopen("Historiques", "a");
    fprintf(fichier, "FIN TOUR NUMERO : %ld \n", Nombre_Tour);
    fclose(fichier);
}

int Manche(fd_set *rdfs)
{
    
    Nombre_Manche++;


    fichier = fopen("Historiques", "a");
    fprintf(fichier, "DEBUT MANCHE NUMERO : %ld \n", Nombre_Manche);
    fclose(fichier);


    Nombre_Tour = 0;
    Construction_Paquet(&p);
    distruCartes(joueurs,&p);
    // On remet les colonnes à zéro et on place une première carte dans chacune.
    for(size_t i = 0 ; i < NOMBRE_LIGNE ; i++)
    {
        Construction_Ligne(Lignes + i);
        Pose_Carte_Ligne(Lignes + i, Tire_Carte(&p));
    }

    for(int i=0 ;i<10;i++)
    {
        //update_fd_set(joueurs,rdfs);
        Tour(joueurs,robots,rdfs);
        //update_fd_set(joueurs,rdfs);
        if (CheckEndGame(joueurs, robots)==FIN) {
            printf("Game over durant le tour numero %d\n", i + 1);
            
            return FIN;
        }
    }
    printf("FIN MANCHE NUMERO : %ld \n",Nombre_Manche);
    fichier = fopen("Historiques", "a");
if (fichier != NULL) {
    fprintf(fichier, "FIN MANCHE NUMERO : %ld \n", Nombre_Manche);
    fclose(fichier);
} else {
    fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
}
    if (fichier != NULL) {
    fprintf(fichier, "FIN MANCHE NUMERO : %ld", Nombre_Manche);
} else {
    fprintf(stderr, "Erreur : Le fichier n'est pas ouvert.\n");
}
    Efface_Ligne(Lignes);
    for(int i=0;i<joueurs.size;i++){
    for(int j=0;j<10;j++)
        {
            joueurs.listeJoueurs[i].main[j].numero=0;
            joueurs.listeJoueurs[i].main[j].tete_boeuf=0;
        }
    }
  
    if(CheckEndGame(joueurs,robots)) return FIN;
    else return CONTINUE;
}

void ExitChat(ListeJoueur *lj, int playerIndex)
{
    //a = 0;
    if(lj->listeJoueurs[playerIndex].c.idEquipe==1) countA++;
    else countB++;
    printf("JOUEUR %s a quitté le chat\n", lj->listeJoueurs[playerIndex].nom);
     lj->listeJoueurs[playerIndex].isInChat = false;
     if(lj->listeJoueurs[playerIndex].c.idEquipe == 1) nbOpenChatA --;
     else nbOpenChatB--;

    // Informer les autres joueurs que ce joueur a quitté le chat
    char exitMessage[BUF_SIZE];
    snprintf(exitMessage, BUF_SIZE, "JOUEUR %s a quitté le chat\n", lj->listeJoueurs[playerIndex].nom);
    
    for (int j = 0; j < lj->size; j++)
    {
        if (lj->listeJoueurs[j].c.idEquipe == lj->listeJoueurs[playerIndex].c.idEquipe &&
            lj->listeJoueurs[j].c.sock != lj->listeJoueurs[playerIndex].c.sock)
        {
            fcntl(lj->listeJoueurs[j].c.sock, F_SETFL, SOCK_NONBLOCK);
            write_client(lj->listeJoueurs[j].c.sock, exitMessage);
        }
    }
}

void send_message_to_Equipe(ListeJoueur lj, Client sender, char *buffer)
{
    int i = 0;
    char message[BUF_SIZE];
    message[0] = 0;
    int une=1;
    histoChatA = fopen("Historique_Chat_A","a");
    histoChatB = fopen("Historique_Chat_B","a");
    printf("Sender: %s, Buffer: %s\n", sender.name, buffer);
    for (i = 0; i < lj.size; i++)
    {
        /* Ne pas envoyer de message à l'expéditeur */
        if (sender.sock != lj.listeJoueurs[i].c.sock && sender.idEquipe == lj.listeJoueurs[i].c.idEquipe && lj.listeJoueurs[i].isInChat)
        {
            printf("Sending to %s\n", lj.listeJoueurs[i].c.name);

            strncpy(message, sender.name, BUF_SIZE - 1);
            strncat(message, " : ", sizeof message - strlen(message) - 1);

            strncat(message, buffer, sizeof message - strlen(message) - 1);
            fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, SOCK_NONBLOCK);
            write_client(lj.listeJoueurs[i].c.sock, message);


            // Écrire dans le fichier histoChat
            if(sender.idEquipe == 1)
            fprintf(histoChatA, "%s\n", message);
            else fprintf(histoChatB, "%s\n", message);
        }
    }
    fclose(histoChatA);
    fclose(histoChatB);
}

void EnvoyerMainAuGroupe(ListeJoueur lj, int joueurActuel)
{
    // Construire une chaîne de caractères représentant la main
    char message[1024];
    message[0] = '\0';
    char carteStr[200];
    // Construire une chaîne de caractères représentant la main sans codes de couleur pour le fichier
    char chatStr[1024];
    chatStr[0] = '\0';
    char carteFileStr[200];

    histoChatA = fopen("Historique_Chat_A","a");
    histoChatB = fopen("Historique_Chat_B","a");


    //CLIENT
    sprintf(carteStr, "\n\t\tMain de %s :\n", lj.listeJoueurs[joueurActuel].nom);
    strncat(message, carteStr, sizeof(message) - strlen(message) - 1);
    snprintf(carteStr,sizeof(carteStr), "\t \t \n %s\n", "\t\tIndice  Numero  Boeuf");
    strncat(message, carteStr, sizeof(message) - strlen(message) - 1);


    //FICHIER
    sprintf(carteFileStr, "\n\t\tMain de %s :\n", lj.listeJoueurs[joueurActuel].nom);
    strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);
    snprintf(carteFileStr, sizeof(carteFileStr), "\t \t \n %s\n", "\t\tIndice  Numero  Boeuf");
    strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);
    for (int i = 0; i < 10; i++)
    {
        
        //Client
        sprintf(carteStr, "\t\t  %d   [ \033[%um%3u  |  %-1u\033[0m ]    \n", i, lj.listeJoueurs[joueurActuel].main[i].Couleur, lj.listeJoueurs[joueurActuel].main[i].numero, lj.listeJoueurs[joueurActuel].main[i].tete_boeuf);
        strncat(message, carteStr, sizeof(message) - strlen(message) - 1);

        //FICHIER
        // Utilisez sprintf pour formater la carte sans les codes de couleur pour le fichier
        sprintf(carteFileStr, "\t\t  %d   [ %3u  |  %-1u ]    \n", i, lj.listeJoueurs[joueurActuel].main[i].numero, lj.listeJoueurs[joueurActuel].main[i].tete_boeuf);
        strncat(chatStr, carteFileStr, sizeof(chatStr) - strlen(chatStr) - 1);
        
    }
    if(lj.listeJoueurs[joueurActuel].c.idEquipe==1) fprintf(histoChatA, "%s\n", chatStr);
        else fprintf(histoChatB, "%s\n", chatStr);

    // Envoyer la main aux autres membres du groupe
    for (int i = 0; i < lj.size; i++)
    {
        if (lj.listeJoueurs[i].c.idEquipe == lj.listeJoueurs[joueurActuel].c.idEquipe && i != joueurActuel)
        {
            write_client(lj.listeJoueurs[i].c.sock, message);
        }
    }
    fclose(histoChatA);
    fclose(histoChatB);
}

void DemanderCarte(ListeJoueur lj)
{   
   
    char buffer[3];
    for(size_t i=0;i<lj.size;i++)
    {
        if(lj.listeJoueurs[i].isInChat == false )
        {
             int flags = fcntl(lj.listeJoueurs[i].c.sock, F_GETFL, 0);
             flags &= ~O_NONBLOCK;  
           fcntl(lj.listeJoueurs[i].c.sock, F_SETFL, flags);        
        Joueur *j = &lj.listeJoueurs[i];
        do
        {   
            
            write_client(lj.listeJoueurs[i].c.sock,"Selectionnez la carte que vous souhaitez jouer : \n");
            read_client(lj.listeJoueurs[i].c.sock, buffer);
            buffer[3]=0;
            if (strlen(buffer) > 0 && !isspace(buffer[0]))
            {
                j->IndiceCarteChoisie = atoi(buffer);

                // Check the range and validity of the chosen card
            }
            memset(buffer, 0, sizeof(buffer));


        }while(!(j->IndiceCarteChoisie >= 1 && j->IndiceCarteChoisie <= 10 && lj.listeJoueurs[i].main[j->IndiceCarteChoisie - 1].numero > 0));
        fichier = fopen("Historiques","a");
        fprintf(fichier, "Joueur %s a choisi la carte [ %u | %u ]\n", lj.listeJoueurs[i].nom, lj.listeJoueurs[i].main[j->IndiceCarteChoisie - 1].numero, lj.listeJoueurs[i].main[j->IndiceCarteChoisie - 1].tete_boeuf);
        j->IndiceCarteChoisie --;
        j->CarteChoisie = j->main[j->IndiceCarteChoisie];
        j->main[j->IndiceCarteChoisie].numero = 0;//On retire la carte (num = 0)
        j->main[j->IndiceCarteChoisie].tete_boeuf = 0;//On retir la carte (boeuf = 0)
        memset(buffer, 0, sizeof(buffer));
        envoyerMainAuClient(lj,i);
        fclose(fichier);
    }   
    }
}

void DemandercarteRobot()
{
 
     //On doit parcourir la liste des robots (robots)

     //printf("%ld\n",robots.size);
    for(size_t i=0;i<robots.size;i++)
    {   
        //printf("Le robot %s va choisir une carte \n",rbt.listeJoueurs[i].nom);
        Joueur *j = &robots.listeJoueurs[i];
     
        srand(time(NULL));

      // Génération d'un nombre aléatoire entre 1 et 3
      int a = rand() % 3 + 1;
    
      int indiceCarte = ChooseCard(Lignes,robots.listeJoueurs[i].main,a);
      
      j->IndiceCarteChoisie = indiceCarte;

      fichier = fopen("Historiques","a");
        fprintf(fichier, "Robot %s a choisi la carte [ %u | %u ]\n", robots.listeJoueurs[i].nom, robots.listeJoueurs[i].main[j->IndiceCarteChoisie - 1].numero, robots.listeJoueurs[i].main[j->IndiceCarteChoisie - 1].tete_boeuf);
     
      j->IndiceCarteChoisie --;
  
        j->CarteChoisie = j->main[j->IndiceCarteChoisie];
      
        j->main[j->IndiceCarteChoisie].numero = 0;//On retire la carte (num = 0)
     
        j->main[j->IndiceCarteChoisie].tete_boeuf = 0;//On retir la carte (boeuf = 0)
        fclose(fichier);
}

}
void AjouteCarteTable(ListeJoueur lj)
{
    for (size_t i = 0; i < lj.size; i++)
    {
        // On récupère le joueur ayant la carte de plus faible valeur.
        // Cette valeur ne devrait jamais être NULL si le nombre d'itérations est correct.
        Joueur *J = JoueurCarteMin(lj);
        printf("Carte choisie par le joueur %s : %d\n", J->nom, J->CarteChoisie.numero);

        for (size_t j = 0; j < NOMBRE_LIGNE; j++)
        {
            int dernierNumero = Lignes[j].Cartes[Lignes[j].taille - 1].numero;
            printf("Lignes[%zu]: taille=%d, carte=%d\n", j, Lignes[j].taille, dernierNumero);
        }
        // On récupère la colonne la plus proche de la carte du joueur mais de valeur inférieure à la carte du joueur.
        Ligne *closestRow = getBonneLigne(Lignes, J->CarteChoisie);

        if (closestRow != NULL) // Si on a trouvé une colonne où le joueur peut poser sa carte
        {
            if (closestRow->taille < 5) // Si la colonne n'est pas complète
                Pose_Carte_Ligne(closestRow, J->CarteChoisie);
            else
            {
                PrendTouteLigne(J, closestRow);
                Pose_Carte_Ligne(closestRow, J->CarteChoisie);
            }
        }
        else // Aucune colonne ne peut accueillir la carte du joueur
        {
            // On doit demander au joueur quelle colonne il souhaite récupérer pour lui.

            int LigneChoisi = 0;
            char buff[2];

            do
            {
                write_client(J->c.sock, "Veuillez choisir une ligne a recuperer { 1 | 2 | 3 | 4 } : ");
                read_client(J->c.sock, buff);
                LigneChoisi = atoi(buff);
                printf("LigneChoisi : %d\n", LigneChoisi);
            } while (!(LigneChoisi >= 1 && LigneChoisi <= 4)); // Pour l'utilisateur, le choix est à faire entre 1 et 4.

            // Pour la machine, les lignes sont numérotées de 0 à 3.
            LigneChoisi--;

            PrendTouteLigne(J, Lignes + LigneChoisi);//Lignes[LigneChoisi]
            Pose_Carte_Ligne(Lignes + LigneChoisi, J->CarteChoisie);
        }

        J->CarteChoisie.numero = 0;
        J->CarteChoisie.tete_boeuf = 0;
    }
 
}

void ajouterCarteRobot()
{
    
     for (size_t i = 0; i < robots.size; i++)
    {
        
        // On récupère le joueur ayant la carte de plus faible valeur.
        // Cette valeur ne devrait jamais être NULL si le nombre d'itérations est correct.
        Joueur *J = &robots.listeJoueurs[i];
       
       if (J != NULL && J->nom != NULL) {
    printf("Carte choisie par le robot %s : %d\n", J->nom, J->CarteChoisie.numero);
} else {
    fprintf(stderr, "Error: Invalid .\n");
}

        for (size_t j = 0; j < NOMBRE_LIGNE; j++)
        {
            int dernierNumero = Lignes[j].Cartes[Lignes[j].taille - 1].numero;
            printf("Lignes[%zu]: taille=%d, carte=%d\n", j, Lignes[j].taille,dernierNumero);
        }
        // On récupère la colonne la plus proche de la carte du joueur mais de valeur inférieure à la carte du joueur.
        Ligne *closestRow = getBonneLigne(Lignes, J->CarteChoisie);

        if (closestRow != NULL) // Si on a trouvé une colonne où le joueur peut poser sa carte
        {
            if (closestRow->taille < 5) // Si la colonne n'est pas complète
                Pose_Carte_Ligne(closestRow, J->CarteChoisie);
            else
            {
                PrendTouteLigne(J, closestRow);
                Pose_Carte_Ligne(closestRow, J->CarteChoisie);
            }
        }
        else // Aucune colonne ne peut accueillir la carte du joueur
        {
            // On doit demander au joueur quelle colonne il souhaite récupérer pour lui.

            int LigneChoisi = 0;

            srand(time(NULL));

            // Génération d'un nombre aléatoire entre 1 et 2
            //int b = rand() % 2 + 1;
            LigneChoisi = (int)ChooseRow(Lignes, robots.listeJoueurs[i].main, 1);
            //Ici j'ai pris le choix 1 parce que c'est le plus optimal
            // Pour la machine, les lignes sont numérotées de 0 à 3.
            LigneChoisi--;

            PrendTouteLigne(J, Lignes + LigneChoisi);
            Pose_Carte_Ligne(Lignes + LigneChoisi, J->CarteChoisie);
        }

        J->CarteChoisie.numero = 0;
        J->CarteChoisie.tete_boeuf = 0;
    }

}

void startGame(ListeClients *lc,fd_set *rdfs,int nbRobots)
{
    //Création du fichier et stockage
    creerFichierEtRepertoire(fichier);
    //Joueurs
    printf("Création des joueurs en cours...\n");
    joueurs.listeJoueurs = (Joueur*)malloc(lc->size * sizeof(Joueur));
    joueurs.size = lc->size;
    robots.size = nbRobots;
    for(int i=0; i <lc->size;i++)
    {
        joueurs.listeJoueurs[i].c = lc->listeClient[i];
        joueurs.listeJoueurs[i].CarteChoisie.numero=0; //Aucune carte choisie
        joueurs.listeJoueurs[i].IndiceCarteChoisie = 0;
        joueurs.listeJoueurs[i].score = 66; 
        FD_SET(joueurs.listeJoueurs[i].c.sock, rdfs);
    }
    robots.listeJoueurs = (Joueur*)malloc(nbRobots * sizeof(Joueur));
     for (int i = 0; i < nbRobots; i++)
    {
        // Initialiser les propriétés du robot
        //joueurs.listeJoueurs[i].c.sock = -1; // Ou toute autre valeur qui indique que c'est un robot interne
        
        robots.listeJoueurs[i].CarteChoisie.numero = 0;
        robots.listeJoueurs[i].IndiceCarteChoisie = 0;
        robots.listeJoueurs[i].score = 66;
    } 
    //printf("%ld\n",robots.size);
    //On demande au joueurs leur nom.
    DemanderNom(joueurs,robots);
    DemanderEquipe(joueurs);
    sleep(1);
    printf("Construction de la table du jeu\n");
    printf("Distrubition de carte\n");
    printf("Maintenant on va communiquer avec les joueurs\n");
    while(Manche(rdfs) == CONTINUE);//Boucle de jeu
    printf("GAME OVER\n");
    clear_clients(lc,joueurs.size);
    free(robots.listeJoueurs);
    free(joueurs.listeJoueurs);
    //system("../script.sh");
}

void creerFichierEtRepertoire(FILE *fichier) {
    // Création d'un répertoire
    const char *nomRepertoire = "Stats";

    // Vérification si le répertoire existe déjà
    struct stat st = {0};
    if (stat(nomRepertoire, &st) == -1) {
        // Le répertoire n'existe pas, alors on le crée
        int statutRepertoire = mkdir(nomRepertoire, 0777);

        // Vérification de la création du répertoire
        if (statutRepertoire == 0) {
            printf("Répertoire créé avec succès.\n");

            // Changement de répertoire de travail
            chdir(nomRepertoire);
        } else {
            fprintf(stderr, "Erreur lors de la création du répertoire.\n");
            return;
        }
    } else {
        // Le répertoire existe déjà
        chdir(nomRepertoire);
    }

    // Ajout de données dans le fichier
    //fprintf(fichier, "Ceci est un exemple de texte dans le fichier.\n");
    printf("Données ajoutées dans le fichier.\n");

     // Ouvrir le fichier
    fichier = fopen("Historiques", "a");
    histoChatA = fopen("Historique_Chat_A","a");
    histoChatB = fopen("Historique_Chat_B","a");

    // Vérifier si le fichier est ouvert avec succès
    if (fichier != NULL) {
        // Ajout de données dans le fichier
        //fprintf(fichier, "Ceci est un exemple de texte dans le fichier.\n");
        printf("Données ajoutées dans le fichier.\n");

        // Fermer le fichier
        fclose(fichier);
    } else {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
    }
}
