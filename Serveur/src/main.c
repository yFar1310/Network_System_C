#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#include "serveur.h"

 void clear_clients(ListeClients *lc, int actual)
{
   int i = 0;
   for(i = 0; i < actual; i++)
   {
      close(lc->listeClient[i].sock);
   }
}


 int init_connection(void)
{
   SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
   SOCKADDR_IN sin = { 0 };

   if(sock == INVALID_SOCKET)
   {
      perror("socket()");
      exit(errno);
   }
 

   sin.sin_addr.s_addr = htonl(INADDR_ANY);
   sin.sin_port = htons(PORT);
   sin.sin_family = AF_INET;

   if(bind(sock,(SOCKADDR *) &sin, sizeof sin) == SOCKET_ERROR)
   {
      perror("bind()");
      exit(errno);
   }
   else
   {
    printf("serveur lié à l'adresse\n");
   }

   if(listen(sock, MAX_CLIENTS) == SOCKET_ERROR)
   {
      perror("listen()");
      exit(errno);
   }
   else
   {
    printf("\t\tà l'écoute des connexions....\n");
   }
   return sock;
}

 void end_connection(int sock)
{
   closesocket(sock);
}

 int read_client(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      //perror("recv()");
      /* if recv error we disonnect the client */
      n = 0;
   }

   buffer[n] = 0;

   return n;
}

 void write_client(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
}

void app(void)
{
   printf("\t\tWelcome\n \t\tI am the server\n");

    int actual;

   do {
        printf("Veuillez entrer le nombre de joueurs (entre 0 et 10) : ");
        
        if (scanf("%d", &actual) != 1) {
            // Si la conversion échoue, le caractère non lu doit être vidé du tampon d'entrée.
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            
            printf("Nombre de joueurs invalide. Veuillez entrer un nombre entre 0 et 10.\n");
        } else if (actual < 0 || actual > 10) {
            printf("Nombre de joueurs invalide. Veuillez entrer un nombre entre 0 et 10.\n");
        }
    } while (actual < 0 || actual > 10);
     int robotCount = 0;
   do
   {
      printf("Veuillez entrer le nombre de robots : ");
      scanf("%d", &robotCount);
      if (robotCount < 0 || robotCount > 10) {
         printf("Nombre de robots invalide. Veuillez entrer un nombre entre 0 et 10.\n");
      }

   } while (robotCount < 0 || robotCount > 10);

    SOCKET sock = init_connection();
    char buffer[BUF_SIZE];
    int max = sock;
    fd_set rdfs;
    ListeClients lc;
    lc.size = actual;
    lc.listeClient = (Client*)malloc(actual * sizeof(Client));
    int i = 0;
    if(actual>0)
    {
       while (1) {
        FD_ZERO(&rdfs);
        FD_SET(sock, &rdfs);

        if (select(max + 1, &rdfs, NULL, NULL, NULL) == -1) {
            
            perror("select()");
            exit(errno);
        }
        if (FD_ISSET(sock, &rdfs)) {
            SOCKADDR_IN csin = {0};
            size_t sinsize = sizeof csin;
            int csock = accept(sock, (SOCKADDR*)&csin, &sinsize);

            if (csock == SOCKET_ERROR) {
                perror("accept()");
                continue;
            }
            printf("socket client %d accepté\n",i+1); 
             /* what is the new maximum fd ? */
            max = csock > max ? csock : max;
            Client c = {csock};
            lc.listeClient[i] = c;
            i++;
        }
        if(actual == i)
        {
        
        printf("Le jeu va commencé\n");
        startGame(&lc,&rdfs,robotCount); 
        clear_clients(&lc, actual);
        end_connection(sock);
    }
    }
    }
    else
    {
      printf("Le jeu va commencé que avec des robots\n");
      startGame(&lc,&rdfs,robotCount);
      clear_clients(&lc, actual);
      end_connection(sock);
    }
   
   
 
         
}
int main()
{
   app();
   
   return 0;
}