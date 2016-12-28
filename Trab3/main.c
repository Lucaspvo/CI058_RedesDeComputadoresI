#include "main.h"

int main (int argc, char** argv) {

    if (argc < 4) {
        printf ("Modo de uso: dgram [localhost port] [localhost letter] [host name] [host port]\n");
        return 0;
        }

    int sock_local = socket_mount ();
    struct sockaddr_in local;
    local_mount (&local, argv[1], sock_local);

    int sock_host = socket_mount ();
        struct sockaddr_in host;
    host_mount (&host, argv[3], argv[4]);

    int token = (argv[5] != NULL) ? 1 : 0;

    server (sock_local, &local, sock_host, &host, token, argv[2]);

    return 0;
}
