#include "main.h"

void server (int sock_local, struct sockaddr_in *local,
                  int sock_host,  struct sockaddr_in *host,
                  int token, char *letra) {
        
		fd_set          input_set;
        struct timeval  timeout;
        int             ready_for_reading = 0;
		int             read_bytes = 0; 
		
        int i = 0, c;
    char *recvbuffer;
    char *sendbuffer;
    int control = 0;

    while (i++ < PACKET_TRIES) {

        if (token > 0) {
        
        			FD_ZERO(&input_set );
					FD_SET(0, &input_set);
					timeout.tv_sec = 20;
					timeout.tv_usec = 0;

					ready_for_reading = select(1, &input_set, NULL, NULL, &timeout);

					if (ready_for_reading == -1) {
					    printf("Unable to read your input\n");
					    control = 1;
					} else {
					    if (ready_for_reading) {
					    	sendbuffer = malloc (BUFFER_LENGTH * sizeof (char));
					        read_bytes = read(0, sendbuffer, 1024);
					        sendbuffer[read_bytes] = '\0';
					    } else {
					        printf(" 20 Seconds are over - no data input \n");
						control = 1;
					    }
	   			 	}
	   			 	if (control == 0){
	   			 		printf("$ ");
	   			 		c = getchar();
	   			 		if (c == '\n'){
	   			 			printf("$ ");
	   			 			c = getchar();
	   			 		}
                        sendbuffer = packet_mount (TYPE_MESSAGE, read_bytes, c, sendbuffer);
                        packet_send (sock_host, host, sendbuffer);
                    } else {
                    	token = 0;
                        recvbuffer[0] = TYPE_TOKEN;
                        printf ("Passando bastao...\n");
                        sleep (PACKET_DELAY);
                        packet_send (sock_host, host, recvbuffer);
                        i = 0;
                        control = 0;
                        continue;
                    }
                }
				recvbuffer = malloc (BUFFER_LENGTH * sizeof (char));
                if ((recvbuffer = packet_recv (sock_local, local))) {
                        if (recvbuffer[0] == TYPE_TOKEN) {
                                token = 1;
                                free(recvbuffer);
                                continue;
                        }
                        
                        if ((recvbuffer[1] == 'b') && (token == 0)) {
                                packet_print (recvbuffer);
                        }
                        
                        if ((recvbuffer[1] == letra[0]) && (token == 0)) {
                                packet_print (recvbuffer);
                        }
                        
                        if (strcmp (recvbuffer, sendbuffer) == 0) {
                                token = 0;
                                recvbuffer[0] = TYPE_TOKEN;
                                printf ("Passando bastao...\n");
                                free(sendbuffer);
                        } else if (token == 1){
                        		token = 0;
                                recvbuffer[0] = TYPE_TOKEN;
                                printf ("Passando bastao...\n");
                                free(sendbuffer);
                        }

						
                        sleep (PACKET_DELAY);
                        packet_send (sock_host, host, recvbuffer);
                        i = 0;
                        free(recvbuffer);        
                        continue;

                } else 
                	printf("Não recebeu\n");

                
                usleep (PACKET_TIME);

    }
}
