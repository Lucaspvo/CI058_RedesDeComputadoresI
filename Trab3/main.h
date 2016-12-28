#include <arpa/inet.h>
#include <errno.h>
#include <math.h>
#include <netdb.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define PACKET_TRIES   16
#define PACKET_TIME    10000
#define PACKET_DELAY   2

#define BUFFER_LENGTH  2048
#define TYPE_TOKEN     'T'
#define TYPE_MESSAGE   'M'

int  socket_mount ();
void host_mount   (struct sockaddr_in *host, char *address, char *port);
void local_mount  (struct sockaddr_in *local, char *port, int sock);
void server         (int sock_local, struct sockaddr_in *local,
                          int sock_host,  struct sockaddr_in *host,
                          int token, char *serv_local);

char* packet_mount (int type, int tam, int server_dest, char* message);
int   packet_send   (int sock_host, struct sockaddr_in *host, char* message);
char* packet_recv   (int sock_local, struct sockaddr_in *local);
void  packet_print  (char* message);
