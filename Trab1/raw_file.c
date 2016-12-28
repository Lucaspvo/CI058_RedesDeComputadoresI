#include "main.h"

void raw_file_send (int sock, char *file_name, FILE *file) {

    //int i=0;
    int buffer_size;
    long int file_size;
    char *tamanho = malloc(sizeof(long int));
    pack packet, packet_recv;
    int status, tries =0;
    unsigned char *buffer = calloc (sizeof(char), PACK_DATA);

        fseek (file, 0, SEEK_END);
        file_size = ftell(file);
        rewind (file);

	sprintf(tamanho, "%ld", file_size);

	packet = mount_pack(TYPE_TAM_ARQ, _seq, (unsigned char *)tamanho, sizeof(tamanho));
	free(tamanho);
	do{
		send_pack(sock, packet);
		if((status = raw_pack_timeout(sock, 1))){
		//printf("passou pelo timeout\n");
			if((packet_recv = recv_pack(sock))){
				if(packet_recv->t_e_p.byte_tp.type == TYPE_OK){
					pack_seq ();
					break;
				}
				else if(packet_recv->t_e_p.byte_tp.type == TYPE_NACK){
					tries++;
					continue;
				}
				else if(packet_recv->t_e_p.byte_tp.type == TYPE_ERRO){
					printf("%s", packet_recv->data);
					return; 
					}
			}
			else
				return;
		}
		else
			tries++;
	}
	while(tries < PACK_TRIES);
	tries = 0;
	printf("Enviado arquivo: %s --------------------------  0Kb", file_name);
	
        while (1) {
				//printf("Enviado arquivo:1\n");
                buffer_size = file_size - ftell(file);

                if (buffer_size <= 0)
                        break;

                if (buffer_size > PACK_DATA)
                        buffer_size = PACK_DATA;
                else
                        buffer[buffer_size] = '\0';

                fread (buffer, 1, buffer_size, file);
		
        
                packet = mount_pack (TYPE_DATA, _seq, buffer, buffer_size);
				printf("\rEnviado arquivo: %s --------------------------   %0.2fKb", file_name, (double)(ftell(file)/1000));
				
				//printf("Enviado arquivo:2\n");
				
		        do{
						send_pack(sock, packet);
						//printf("Enviado arquivo:after send \n");
						if((status = raw_pack_timeout(sock, 1))){
						
							if((packet_recv = recv_pack(sock))){
								if(packet_recv->t_e_p.byte_tp.type == TYPE_ACK){
									pack_seq ();
									break;
								}	
								else if(packet_recv->t_e_p.byte_tp.type == TYPE_NACK){
									tries++;
									continue;
								}
							}
						}
						else
							tries++;
					
					}
					while(tries < PACK_TRIES);
		//printf("%d\n", i++);
		free(packet);
		tries = 0;
        }

        

        packet = mount_pack (TYPE_END_U, _seq, NULL, 0);
                
        do{
		send_pack(sock, packet);
		if((status = raw_pack_timeout(sock, 1))){
		//printf("passou pelo timeout\n");
			if((packet_recv = recv_pack(sock))){
				if(packet_recv->t_e_p.byte_tp.type == TYPE_ACK){
					pack_seq ();
					break;
				}
				else if(packet_recv->t_e_p.byte_tp.type == TYPE_NACK){
					tries++;
					continue;
				}
			}
		}
		else
			tries++;
	}
	while(tries < PACK_TRIES);
	tries = 0;
	printf("\n");
	free(buffer);
}

void raw_file_recv (int sock, char *file_name, FILE *file) {

        long int file_size = 0;
        pack packet, packet_recv; 
        int status, tries =0;
        struct statvfs *vfs = malloc(sizeof(struct statvfs));
        long long HD_size;

    
    
    packet_recv = recv_pack(sock); //tamanho do arquivo a ser recebido.
    
    if(packet_recv->t_e_p.byte_tp.type == TYPE_TAM_ARQ){
        statvfs("/", vfs);
        HD_size = vfs->f_bsize - file_size;
        if (HD_size <= 0){
            char *errostr = malloc(sizeof(char)*32);
            strcpy (errostr, "Tamanho em disco insuficiente\n");
            packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)errostr, strlen(errostr));
            send_pack(sock, packet);
            return;
        }
        else{
            packet = mount_pack(TYPE_OK, _seq, NULL, 0);
            send_pack(sock, packet);
            pack_seq();
        }
    }
    else
        return; // Caso não receba o tamanho.
	printf("Recebendo arquivo: %s --------------------------   0Kb", file_name);
	packet_recv = recv_pack(sock);
	while (1) {
			
			if (packet_recv->t_e_p.byte_tp.type == TYPE_DATA){
					fwrite (packet_recv->data, 1, packet_recv->s_e_s.byte_ss.size, file);
					printf("\rRecebendo arquivo: %s --------------------------   %0.2fKb", file_name, (double)(ftell(file)/1000));
					packet = mount_pack (TYPE_ACK, _seq, NULL, 0);
					do{
						
						send_pack (sock, packet);

						if((status = raw_pack_timeout(sock, 1))){
							pack_seq();
							if((packet_recv = recv_pack(sock)))
								break;
							else
								return;
						}
						else
							tries++;        

					}
					while(tries < PACK_TRIES);   
					
			}
				

			else if (packet_recv->t_e_p.byte_tp.type == TYPE_END_U) {
					packet = mount_pack (TYPE_ACK, _seq, NULL, 0);
					send_pack (sock, packet);
					pack_seq();
					printf ("\n########### Transferencia Finalizada ###########\n");
					//fclose (file);
					free(packet_recv);
					printf("saiu\n");
					return;
			}
	}

   // fseek (file, 0, SEEK_END);
    //if (file_size != ftell(file))
     //           printf ("Tamanhos diferentes\n");
    //    else
    

        

}
