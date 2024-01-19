#include "inclusions.h"
#include "Client.h"

 void app(void);
 int init_connection(void);
 void end_connection(int sock);
 int read_client(SOCKET sock, char *buffer);
 void write_client(SOCKET sock, const char *buffer);
 void clear_clients(ListeClients *lc, int actual);
 
 