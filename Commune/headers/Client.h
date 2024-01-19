#ifndef CLIENT_H
#define CLIENT_H

#include "inclusions.h"
#include <stdarg.h>



typedef struct
{
   SOCKET sock;
   char name[BUF_SIZE];
   int idEquipe;
}Client;

typedef struct ListeClients
{
    Client *listeClient;
    int size;
    int capa;
}ListeClients;

//Méthodes pour gérer l'envoie et la réception des messages

#endif 