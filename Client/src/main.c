#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "client.h"


static void app()
{
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == -1)
    {
        perror("gethostname()");
        exit(errno);
    }

    printf("Hostname: %s\n", hostname);

    SOCKET sock = init_connection(hostname);
    char buffer[BUF_SIZE];

    fd_set rdfs;

    char name[BUF_SIZE];
    char groupe[BUF_SIZE];
    int etat=0;
    while (1)
    {
        
        FD_ZERO(&rdfs);

        /* add STDIN_FILENO */
        FD_SET(STDIN_FILENO, &rdfs);

        /* add the socket */
        FD_SET(sock, &rdfs);

        if (select(sock + 1, &rdfs, NULL, NULL, NULL) == -1)
        {
            perror("select()");
            exit(errno);
        }

        /* something from standard input : i.e keyboard */
        if (FD_ISSET(STDIN_FILENO, &rdfs))
        {
    if (fgets(buffer, BUF_SIZE - 1, stdin) != NULL) {
        // Supprimer le caractère de nouvelle ligne s'il est présent
        char *p = strchr(buffer, '\n');
        if (p != NULL) {
            *p = '\0';
        }

        // Vérifier si la chaîne contient des caractères significatifs
        int i;
        for (i = 0; buffer[i] != '\0'; i++) {
            if (!isspace((unsigned char)buffer[i])) {
                // La chaîne contient des caractères significatifs, l'envoyer au serveur
                write_server(sock, buffer);
                //printf("Envoi du message au serveur réussi\n");
                sleep(1);
                break;
            }
        }
    }
        
        }
        else if (etat == 0)
        {
            int n = read_server(sock, buffer);
            /* server down */
            if (n == 0)
            {
                printf("Server disconnected 11!\n");
                break;
            }
            puts(buffer);
            scanf("%s", name);
            getchar();
            // Envoyer le nom au serveur
            write_server(sock, name);
            etat = 1;
        }
        else if(etat == 1)
        {
             int n = read_server(sock, buffer);
            /* server down */
            if (n == 0)
            {
                printf("Server disconnected 22!\n");
                break;
            }
            puts(buffer);
            scanf("%s", groupe);
            // Envoyer le groupe au serveur
            write_server(sock, groupe);
            fflush(stdin);
            etat = 2;
        }
        
        else if(FD_ISSET(sock, &rdfs))
      {
         int n = read_server(sock, buffer);
         /*
         //server down 
         if(n == 0)
         {
            printf("Server disconnected !!!!\n");
            break;
         }*/
         puts(buffer);
      }
    }
    end_connection(sock);
}

static int init_connection(const char *hostname)
{
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    SOCKADDR_IN sin = {0};
    struct hostent *hostinfo;

    if (sock == INVALID_SOCKET)
    {
        perror("socket()");
        exit(errno);
    }

    hostinfo = gethostbyname(hostname);
    if (hostinfo == NULL)
    {
        fprintf(stderr, "Unknown host %s.\n", hostname);
        exit(EXIT_FAILURE);
    }

    sin.sin_addr = *(IN_ADDR *) hostinfo->h_addr;
    sin.sin_port = htons(PORT);
    sin.sin_family = AF_INET;

    if (connect(sock, (SOCKADDR *)&sin, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        perror("connect()");
        exit(errno);
    }

    return sock;
}

static void end_connection(int sock)
{
   closesocket(sock);
}

static int read_server(SOCKET sock, char *buffer)
{
   int n = 0;

   if((n = recv(sock, buffer, BUF_SIZE - 1, 0)) < 0)
   {
      perror("recv()");
      exit(errno);
   }

   buffer[n] = 0;

   return n;

}

static void write_server(SOCKET sock, const char *buffer)
{
   if(send(sock, buffer, strlen(buffer), 0) < 0)
   {
      perror("send()");
      exit(errno);
   }
   
}

int main(int argc, char **argv)
{
   
   app();

   return EXIT_SUCCESS;
}