#include "main.h"

int _seq = 0;

pack mount_pack(int type, int sequence, unsigned char *data, int size_data){
	//printf("merda dentro do mount\n\n\n");
        pack packet = malloc (sizeof(char)*(size_data +3));
        

        packet->init   = PACK_INIT;
        packet->s_e_s.byte_ss.size   = size_data;
        packet->s_e_s.byte_ss.seq    = sequence;
        packet->t_e_p.byte_tp.type   = type;
        packet->data   = data;
        packet->t_e_p.byte_tp.parity = parity_pack(packet); 
	//printf("merda final mount\n\n\n");
        return (packet);
}


int parity_pack(pack packet){
	//printf("merda dentro do parity\n\n\n");
    int i;
    int par = 0;
    union byte parity_data;
	        
	for (i = 0; i < packet->s_e_s.byte_ss.size; i++ ){
		parity_data.value = packet->data[i];
           	par ^= parity_data.byte.four_first;
           	par ^= parity_data.byte.four_last;
    }
    return 1;// par; //par;

}

unsigned char* packet_to_string (pack packet){

	int i=0;
	unsigned char *string = malloc(sizeof(unsigned char)*(packet->s_e_s.byte_ss.size+3));
	
	string[0] = packet->init;
	string[1] = packet->s_e_s.value;
	string[2] = packet->t_e_p.value;
	for (i = 0; i < packet->s_e_s.byte_ss.size; i++){
		string[i+3] = packet->data[i];
		
	}

	return string;

}

pack string_to_packet (unsigned char *string){

	int i=0;
	union size_seq size;
	size.value = string[1];
	pack packet = malloc(sizeof(unsigned char)*(size.byte_ss.size + 3));

	packet->init = string[0];
	packet->s_e_s.value = string[1];
	packet->t_e_p.value = string[2];
	
	packet->data = calloc(sizeof(unsigned char), packet->s_e_s.byte_ss.size);
	for (i = 0; i < packet->s_e_s.byte_ss.size; i++){
		packet->data[i] = string[i+3];
		//printf("%c", packet->data[i]);
		
	}

	
	packet->data[packet->s_e_s.byte_ss.size] = '\0';
	//printf("\n::::%s\n", packet->data);
	packet->t_e_p.byte_tp.parity = parity_pack(packet);
	//printf("\n@@@@::::%s\n", packet->data);

	return packet;

}

int send_pack (int socket, pack packet){

	
	unsigned char *string;
	while(1){
		
		string = packet_to_string(packet);
		
			
		if (send(socket, string, PACK_SIZE_MAX, 0) == -1){
			//printf("erro: send: %s\n", strerror(errno));
			continue;
		}
		free(string);
		return 1;
	}

}

pack recv_pack (int socket){

    
    pack packet;
   
    
	unsigned char *received = malloc (sizeof(char)*PACK_SIZE_MAX);

    while(1){
		 
		
		if ((recv (socket, received, PACK_SIZE_MAX, 0)) < 0) {
		        printf("erro: recv: %s\n", strerror(errno));
		        continue;
		}
		//printf("antes strin 2 pak:\n\n\n");
		packet = string_to_packet(received);
		//printf("depois strin 2 pak:\n\n\n");
	//	printf("\n::::%s\n", packet->data);

		if (packet->init != PACK_INIT){ 
			//printf("Erro no Init\n\n\n");
			//packet = mount_pack (TYPE_NACK, _seq, NULL, 0);
			//send_pack(socket, packet);    
			continue;
		}

		if (parity_pack(packet) != packet->t_e_p.byte_tp.parity){
			printf("Erro de Paridade entre os pacotes\n\n\n");
			packet = mount_pack (TYPE_NACK, _seq, NULL, 0);
			send_pack(socket, packet);
			continue;
		}
		if (packet->s_e_s.byte_ss.seq != _seq){
			printf("Erro de Sequencia dos pacotes\n\n\n");
			//printf("\nrecebido: %d ----- meu: %d\n", packet->s_e_s.byte_ss.seq, _seq);
			anti_pack_seq(packet);
			packet = mount_pack (TYPE_NACK, _seq, NULL, 0);
			send_pack(socket, packet);
			continue;
		}
		//printf("aqui Recebendo arquivo:\n\n\n");
		free(received);
		//printf("aqui Recebendo arquivo:\n\n\n");
		return packet;
	}
    return 0;
}

int raw_pack_timeout(int socket, int offset){


	struct pollfd poll_descriptor;
	poll_descriptor.fd = socket;
    poll_descriptor.events = POLLIN;
    int poll_ret;


    	poll_ret = poll(&poll_descriptor, 1, (PACK_TIMEOUT*offset));
		if( poll_ret > 0)
		{
			return 1;
		}
		else{
			printf("no timeout\n");
			return 0;
		}			
	
}

void pack_seq(){
	
	if (_seq < (PACK_SEQ - 1))
		_seq ++;
	else {
		//printf("Entrou no else pack_seq");
		_seq = 0;
	}

}

void anti_pack_seq(pack packet_recv){

	if(_seq == 0)
		_seq = 3;
	else
		_seq--;

}
		


	





















