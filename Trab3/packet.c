#include "main.h"

char* packet_mount (int type, int tam, int server_dest, char* message) {
        char *buffer = (char*) malloc ((strlen(message) * sizeof (char) + 1));
        buffer[0] = type;
        buffer[1] = server_dest;
        buffer[2] = tam - 1;
        strcpy (buffer + 3, message);
        return buffer;
}

void packet_print (char* message) {
        int i = 3;
        printf("Mensagem: ");
        while (message[i] != '\0')
                printf("%c", message[i++]);
}

int packet_send (int sock_host, struct sockaddr_in *host, char* message) {

        int s;
        int length = sizeof (struct sockaddr_in);

        if ((s = sendto (sock_host, message, strlen (message), 0,
                (struct sockaddr *) host, (socklen_t) length)) < 0) {
                perror("sendto");
                return 0;
        }

        return 1;
}

char* packet_recv (int sock_local, struct sockaddr_in *local) {

        int n;
        int length = sizeof (struct sockaddr_in);
        char *buffer = (char*) malloc (BUFFER_LENGTH * sizeof(char));

        printf ("Esperando dados...\n");

        if ((n = recvfrom (sock_local, buffer, BUFFER_LENGTH, 0,
                (struct sockaddr *) local, (socklen_t *) &length) < 0)) {
                perror("recfrom");
                return 0;
        }

        write(1, buffer, n);
        return buffer;

}
