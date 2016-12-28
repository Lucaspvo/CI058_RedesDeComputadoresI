#include "main.h"

int socket_mount () {

        int sock;

    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
        perror("socket");

        return sock;
}

void host_mount (struct sockaddr_in *host, char *address, char *port) {

    struct hostent *hp;
        host->sin_family = AF_INET;

        if ((hp = gethostbyname (address)) <= 0)
                printf ("erro: nao pode encontrar o servidor\n");

        memcpy ((char *) &host->sin_addr, (char *) hp->h_addr, hp->h_length);
        host->sin_port = htons (atoi (port));

        printf("Conectado ao servidor %s na porta %d\n", inet_ntoa (host->sin_addr), host->sin_port);
}

void local_mount (struct sockaddr_in *local, char *port, int sock) {

        int length;
        length = sizeof (struct sockaddr_in);
        bzero (local, length);
        local->sin_family = AF_INET;
        local->sin_addr.s_addr = INADDR_ANY;
        local->sin_port = htons(atoi(port));
        bzero (&(local->sin_zero), 8);

        if (bind (sock, (struct sockaddr *) local, length) < 0)
                perror ("bind");

        printf ("Ouvindo a porta %d\n", local->sin_port);
}
