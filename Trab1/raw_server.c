#include "main.h"

void raw_server_init(){
		char dir_name[61];
        int sock_server;
        char *msg = malloc(sizeof(char)*40); // Remover 
				pack packet;

                sock_server = create_sock(SOCKET_SERVER_DEVICE);

                recv(sock_server, msg, sizeof(char)*40, 0); //Remover até o while
                printf("%s\n", msg);
                free(msg);

                msg = malloc(sizeof(char)*40);
                strcpy(msg, "Conectado com o server");
                send(sock_server, msg, sizeof(char)*40, 0);
                free(msg);
                getcwd(dir_name, sizeof(dir_name));
                packet = mount_pack(TYPE_DIR_NAME, _seq, (unsigned char *)dir_name, strlen(dir_name));
                send_pack(sock_server, packet);
                while(1){

                		

                		printf("MAIN SERVER: ");
                        packet = recv_pack(sock_server);
			
			
                        if (packet->t_e_p.byte_tp.type == TYPE_CD){
							printf("entrou CD\n");
                        	raw_server_cd(sock_server, (unsigned char *)packet->data);
   							getcwd(dir_name, sizeof(dir_name));
                			packet = mount_pack(TYPE_DIR_NAME, _seq, (unsigned char *)dir_name, strlen(dir_name));
                			send_pack(sock_server, packet);
                        }
                        else if (packet->t_e_p.byte_tp.type == TYPE_LS){
							printf("entrou LS\n");
                        	raw_server_ls(sock_server, (unsigned char *)packet->data);
                        }
                        else if (packet->t_e_p.byte_tp.type == TYPE_PUT){
							printf("entrou PUT\n");
	                        raw_server_put(sock_server, (unsigned char *)packet->data);
                 			free (packet);
                 	    }
                        else if (packet->t_e_p.byte_tp.type == TYPE_GET){
							printf("entrou GET");
        	                raw_server_get(sock_server, (unsigned char *)packet->data);
                        }
				}
             
}
        
void raw_server_cd(int sock_server, unsigned char *dir){

	char cwd[1024]; 
  	pack packet;
	    if ((chdir ((const char *) dir)) == 0) { //Tenta abrir o diretório
	    	getcwd (cwd, sizeof(cwd)); // Pega o nome do diretório corrente
			packet = mount_pack(TYPE_OK, _seq, NULL, 0); //Monta o pacote com OK

		}
		else{
			char *erro = malloc(sizeof(int)*3);
			sprintf (erro, "%d", errno); 
			packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno)); //Monta o pacote com o erro passado pelo errno
		}
		send_pack(sock_server, packet);// Envia o Pacote com OK ou ERRO
		pack_seq();
}


void raw_server_ls(int sock_server, unsigned char *opt){
        
	pack packet;
	printf("Opção recebida: %s\n", opt);
	if (!strcmp((char *)opt, "ls -l")){
		FILE *file_LSL;
		system("ls -l > .ls_l"); 	//Cria o arquivo com o ls -l do servidor
		file_LSL = fopen(".ls_l", "r"); //Le o arquivo criado que possui o ls -l
		
		raw_send_ls(sock_server, file_LSL);  //Envia o ls -l para o cliente
				
		fclose(file_LSL);	//Fecha o arquivo criado
		remove(".ls_l");	//Deleta o arquivo criado pelo system("");

		packet = mount_pack(TYPE_END_U, _seq, NULL, 0); //Cria o pacote com o final de transmissão 	 
		send_pack(sock_server, packet); //Envia o final de transmissão

	}
	if (!strcmp((char *)opt, "ls")){
		FILE *file_LS;
		system("ls > .ls");		//Cria o arquivo com o ls do servidor
		file_LS = fopen(".ls", "r");	//Le o arquivo criado
				
		raw_send_ls(sock_server, file_LS);	//Envia o ls para o cliente
		
		fclose(file_LS);	//Fecha o arquivo criado
		remove(".ls");		//Deleta o arquivo

		packet = mount_pack(TYPE_END_U, _seq, NULL, 0); //Cria o pacote com o final de transmissao
		send_pack(sock_server, packet);	//Eniva o final de transmissão para o cliente
		
	}
	return;
}

void raw_server_put(int sock_server, unsigned char *file_name){
        printf("entrou PUT\n");
       
        int status, tries;
        pack packet, packet_recv;
        FILE *file;
       	if(file_name[0] != '*'){
		if ((file = fopen ((char *)file_name, "w")) == NULL) {
			printf ("PUT: Não foi possivel criar arquivo\n");
			char *erro = malloc(sizeof(int)*3);
				sprintf (erro, "%d", errno);
			packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno));
			send_pack(sock_server, packet);
			return;
		}
		else if (file != NULL){
			packet = mount_pack(TYPE_OK, _seq, NULL, 0);
			send_pack(sock_server, packet);
			//printf("%d\n", _seq);
			pack_seq();
		}
		raw_file_recv(sock_server, (char *)file_name, file);
	}
	else{
			
		packet = mount_pack(TYPE_OK, _seq, NULL, 0);
		do{
			send_pack(sock_server, packet);
			pack_seq();
			if((status = raw_pack_timeout(sock_server, 1))){
				packet_recv= recv_pack(sock_server);
				break;
			}
			else
				tries++;
		}
		while(tries < PACK_TRIES);
		tries = 0;
		while(1){
				if (packet_recv->t_e_p.byte_tp.type == TYPE_FILE_NAME){
					packet = mount_pack(TYPE_OK, _seq, NULL, 0);
					send_pack(sock_server, packet);
					pack_seq();
				if ((file = fopen ((char *)packet_recv->data, "w")) == NULL) {
					printf("file: arquivo inexistente\n");
					return;
   				}
				raw_file_recv(sock_server, (char *)packet_recv->data, file);
				//fclose(file);
				do{
					if((status = raw_pack_timeout(sock_server, 1))){
						packet_recv= recv_pack(sock_server);
						break;
					}
					else
						tries++;
				}
				while(tries < PACK_TRIES);
				tries=0;
			}
			else if (packet_recv->t_e_p.byte_tp.type == TYPE_END_GR){
				printf ("\n########### Transferencia Finalizada ###########\n");
				return;
			}
		}
		
	}
}

void raw_server_get(int sock_server, unsigned char *file_name){
        pack packet, packet_recv;
	FILE *file;
	char *name = malloc(sizeof(char)*30);
	char *ls = malloc(sizeof(char)*30);
	
	FILE *fp;
	
	
	if (file_name[0] != '*'){
		printf("%s\n", file_name);
		if ((file = fopen ((char *)file_name, "r")) == NULL) {
			printf ("GET: Não foi possivel abrir o arquivo\n");
			char *erro = malloc(sizeof(int)*3);
				sprintf (erro, "%d", errno);
			packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno));
			send_pack(sock_server, packet);
			return;
        	}
		raw_file_send(sock_server, (char *)file_name, file);
	}
	else{
		sprintf (ls, "ls %s > .ls", file_name);
		system(ls);
		if ((file = fopen (".ls", "r")) == NULL) {
			printf ("GET: Não foi possivel abrir o arquivo\n");
			char *erro = malloc(sizeof(int)*3);
				sprintf (erro, "%d", errno);
			packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno));
			send_pack(sock_server, packet);
			return;
        	}
		printf("VAI ENTRAR WHILE>>>>>\n");
		while (fscanf(file, "%s", name) != EOF){
			if ((fp = fopen (name, "r")) == NULL) {
				printf ("GET: Não foi possivel abrir o arquivo\n");
				char *erro = malloc(sizeof(int)*3);
					sprintf (erro, "%d", errno);
				packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno));
				send_pack(sock_server, packet);
				return;
        		}
			packet = mount_pack(TYPE_FILE_NAME, _seq, (unsigned char *)name, strlen(name));
			send_pack(sock_server, packet);
			packet_recv = recv_pack(sock_server);
			if (packet_recv->t_e_p.byte_tp.type == TYPE_OK){
				pack_seq();
				raw_file_send (sock_server, name, fp);
				printf("SAIU>>>>>>>>>\n");
			}
			fclose(fp);		
		}
		packet = mount_pack(TYPE_END_GR, _seq, NULL, 0);
		send_pack(sock_server, packet);
	}
		fclose (file);
		return;
}
