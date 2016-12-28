#include "main.h"

void raw_send_ls (int sock, FILE *file) {

    int bufflen = 1;
    int status, tries = 0;
    unsigned char* buffer = malloc (PACK_DATA);
    pack packet, packet_recv;
	
        fseek (file, 0, SEEK_END);
        long int filesize = ftell(file);
        
        rewind (file);
        while (1) {
                printf("while 1\n");
                bufflen = filesize - ftell(file);

                if (bufflen <= 0)
                        break;

                if (bufflen > PACK_DATA)
                        bufflen = PACK_DATA;
                else
                        buffer[bufflen] = '\0';

                fread (buffer, 1, bufflen, file);

                packet = mount_pack (TYPE_SHOW, _seq, buffer, bufflen);
                do{
                    send_pack (sock, packet);
                    if((status = raw_pack_timeout(sock, 2))){
                        //printf("entrou no timeout\n");
                        if((packet_recv = recv_pack(sock))){
                            //printf("recebeu\n");
                            if(packet_recv->t_e_p.byte_tp.type == TYPE_ACK){
								pack_seq ();
                                break;
                            }
                            else if (packet_recv->t_e_p.byte_tp.type == TYPE_NACK)
                                continue;
                        }
                        else
                            return;
                    }
                    else
                        tries++;
                }
                while(tries < PACK_TRIES && status == 0);
                if ((tries == PACK_TRIES) && (status == 0)){
                    printf("Conexão perdida\n");
                    return;
                }
                
        }
        printf("SAIU\n");
        free(buffer);
    	return;

}
