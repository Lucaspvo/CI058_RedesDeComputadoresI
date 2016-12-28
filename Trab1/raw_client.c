#include "main.h"


void raw_client_init(){

        int sock_client;
		int i;
        char c;
		char *option = (char *) calloc(sizeof(char**), 65);
		char **opt = (char **) calloc (sizeof(char**), 20);
        char *msg = malloc(sizeof(char)*40); // Remover 
		pack remote_dir_name;

                sock_client = create_sock(SOCKET_CLIENT_DEVICE);

                strcpy(msg, "Conectado com o cliente"); //Remover até o while
                send(sock_client, msg, strlen(msg), 0);
                free(msg);
                
                msg = malloc(sizeof(char)*40);
                recv(sock_client, msg, sizeof(char)*40, 0);
                printf("%s\n", msg);
                free(msg);

				remote_dir_name = recv_pack(sock_client);
				char* dir_name = malloc(sizeof(char)*63);
				sprintf (dir_name, "%s", remote_dir_name->data);
                while(1){

		 
			
                  	printf ("[user@server %s]:> ", dir_name);
					i = 0;
					if ((c = getchar()) == '\n')
						continue;
					do                	
						option[i++] = c;
					while ((c = getchar ()) != '\n');                	
							option[i] = '\0';
                                     
					i = 0;
					opt[i] = strtok (option, " ");
					while((opt[++i] = strtok(NULL, " ")));
					if (!strcmp(opt[0], ".q")){
						return;
					} 
                    else if (!strcmp(opt[0], "cd")){
						raw_client_cd(sock_client, opt[1]);
						remote_dir_name = recv_pack(sock_client);
						sprintf (dir_name, "%s", remote_dir_name->data);
					}
                    else if (!strcmp(opt[0], "ls")){
						if(opt[1]){
							sprintf(option, "%s %s", opt[0], opt[1]);
							if(strcmp("ls -l", option))
								continue;
						}
					    raw_client_ls(sock_client, option);
					}
					else if (!strcmp(opt[0], "put")){
						raw_client_put(sock_client, opt[1]);
					}
					else if (!strcmp(opt[0], "get")){
						raw_client_get(sock_client, opt[1]);
					}
					else if (!strcmp(opt[0], "list")){ //ls local
						if(opt[1])
							sprintf(option, "%s %s", opt[0], opt[1]);
						raw_client_local_ls(option);
					}
					else if (!strcmp(opt[0], "open")){ //cd local
						raw_client_local_cd(opt[1]);
					}
                } 
}

void raw_client_cd(int sock_client, char *folder){
        
  	if (!folder){
		printf("cd: forneça o nome de um diretório");
		return;
	}
	
	pack packet, packet_recv;
	int str = 0, status, tries = 0;
	
	packet = mount_pack(TYPE_CD, _seq, (unsigned char *)folder, strlen(folder));
	do{
		send_pack(sock_client, packet);
		if((status = raw_pack_timeout(sock_client, 1))){
			if((packet_recv = recv_pack(sock_client))){
				if((packet_recv->t_e_p.byte_tp.type != TYPE_OK)){
					if(packet_recv->t_e_p.byte_tp.type == TYPE_NACK){
						tries++;
						continue;
					}
					else if (packet_recv->t_e_p.byte_tp.type == TYPE_ERRO){
						str = atoi((const char*)packet_recv->data);
						switch(str){
			     			case EACCES:{ 
								printf("Permissão Negada\n");
								return;
							}
			      			case ENAMETOOLONG:{
								printf("O PATH é muito grande\n");
								return;
							}
						    case ENOTDIR:{
								printf("Um componente do PATH não é um diretório\n");
								return; 
							}
						    case ENOENT:{ 
								printf("O diretório não existe\n"); 
								return;
							}
						    default:{ 
								printf("Não foi possível mudar para o diretório %s\n", folder);
								return;
							}
						}
					}
				}
				else if (packet_recv->t_e_p.byte_tp.type == TYPE_OK){
					pack_seq ();
					break;
				}
			}
			else
				return;
		}
		else
			tries++;
			continue;
	}
	while(tries < PACK_TRIES && status == 0);

	if ((tries == PACK_TRIES) && (status == 0)){
		printf("Conexão perdida\n");
	}
 
	return;

}



void raw_client_ls(int sock_client, char *LS){
  	
	pack packet, packet_recv;
	
	int tries = 0, status;

	packet = mount_pack(TYPE_LS, _seq, (unsigned char *)LS, strlen(LS));
	do{
		send_pack(sock_client, packet);
		if((status = raw_pack_timeout(sock_client, 1)))
			if((packet_recv = recv_pack(sock_client)))
				break;
			else
				return;
		else
			tries++;
			continue;
	}
	while(tries < PACK_TRIES && status == 0);

	if ((tries == PACK_TRIES) && (status == 0)){
		printf("Conexão perdida\n");

	}

	tries = 0;
	status = 0;
	printf("\nLS Servidor:\n");
	do{
		if (packet_recv->t_e_p.byte_tp.type != TYPE_END_U){
			if (packet_recv->t_e_p.byte_tp.type == TYPE_SHOW){
				printf("%s", packet_recv->data);
				packet = mount_pack (TYPE_ACK, _seq, NULL, 0);
				send_pack (sock_client, packet);

				
			}
			if((status = raw_pack_timeout(sock_client, 1))){
				pack_seq();
				if((packet_recv = recv_pack(sock_client))){
					continue;
				}
				else{
					return;
				}
			}
			else{
				tries++;
			}
		}
		else {
			printf("\n");
			break;
		}
	}
	while(tries < PACK_TRIES);
	return;

}

void raw_client_get(int sock_client, char *file_name){

	int status, tries = 0;
	pack packet, packet_recv;
	
	
	FILE *file;
		if(file_name[0] != '*'){
			printf("%s\n", file_name);
			if ((file = fopen (file_name, "w")) == NULL) {
        			printf("file: arquivo inexistente\n");
        			return;
   			}
			packet = mount_pack(TYPE_GET, _seq, (unsigned char*)file_name, strlen(file_name));
			printf("%s\n", packet->data);
			do{
				send_pack(sock_client, packet);
				if((status = raw_pack_timeout(sock_client, 1))){
					raw_file_recv(sock_client, file_name, file);
					fclose(file);		
					return;
				}
				else
					tries++;
			}
			while(tries < PACK_TRIES);
			printf ("\n########### Transferencia Finalizada ###########\n");
		}
		else{
			
			packet = mount_pack(TYPE_GET, _seq, (unsigned char*)file_name, strlen(file_name));
			do{
				send_pack(sock_client, packet);
				if((status = raw_pack_timeout(sock_client, 1))){
					packet_recv= recv_pack(sock_client);
					break;
				}
				else
					tries++;
			}
			while(tries < PACK_TRIES);
			tries = 0;
			while(1){
					if (packet_recv->t_e_p.byte_tp.type == TYPE_FILE_NAME){
						printf("aqui Recebendo arquivo:\n\n\n");
						packet = mount_pack(TYPE_OK, _seq, NULL, 0);
						printf("aqui Recebendo arquivo:\n\n\n");
						send_pack(sock_client, packet);
						
						pack_seq();
						packet_recv->data[packet_recv->s_e_s.byte_ss.size] = '\0';
						if ((file = fopen ((char *)packet_recv->data, "w")) == NULL) {
							printf("file: arquivo inexistente\n");
							return;
	   				}
	   				printf("ante raw_file aqui Recebendo arquivo:\n\n\n");
					raw_file_recv(sock_client, (char*)packet_recv->data, file);
					fclose(file);
					do{
						if((status = raw_pack_timeout(sock_client, 1))){
							printf("aqui Recebendo arquivo:\n\n\n");
							packet_recv= recv_pack(sock_client);
							printf("depois pack_recv:\n\n\n");
							break;
						}
						else
							tries++;
					}
					while(tries < PACK_TRIES);
					tries=0;
				}
				else if (packet_recv->t_e_p.byte_tp.type == TYPE_END_GR){
					return;
				}
			}
			printf ("\n########### Transferencia Finalizada ###########\n");
		}
		
}

void raw_client_put(int sock_client, char *file_name){
        
	int status, str = 0, tries = 0;
	pack packet, packet_recv;
	FILE *file, *fp;
	char *name = malloc(sizeof(char)*30);
	char *ls = malloc(sizeof(char)*10);
		
		if(file_name[0] != '*'){
			
    			if ((file = fopen (file_name, "r")) == NULL) {
        			printf("file: arquivo inexistente\n");
        			return;
   			}
			
			packet = mount_pack(TYPE_PUT, _seq, (unsigned char*)file_name, strlen(file_name));
       			do{
				send_pack(sock_client, packet);
				if((status = raw_pack_timeout(sock_client, 1))){
					if((packet_recv = recv_pack(sock_client))){
						if(packet_recv->t_e_p.byte_tp.type == TYPE_OK){
							pack_seq ();
							break;
						}
						else if(packet_recv->t_e_p.byte_tp.type == TYPE_NACK){
							tries++;
							continue;
						}
						else if(packet_recv->t_e_p.byte_tp.type == TYPE_ERRO){
							str = atoi((const char*)packet_recv->data);
							switch(str){
			     					case EINVAL:{
									printf("Erro ao tentar criar arquivo no servidor, erro: %d\n",str);
									return;
								}
								default:{
									printf("Erro ao tentar criar arquivo no servidor, erro desconhecido\n");
									return;
								} 
							}
						}
					}
				}
				else
							tries++;
					}
					while(tries < PACK_TRIES);
					raw_file_send(sock_client, file_name, file);
					fclose (file);
					return;
		}
		else{
			sprintf (ls, "ls %s > .ls", file_name);
			system(ls);
			if ((file = fopen (".ls", "r")) == NULL) {
				printf ("GET: Não foi possivel abrir o arquivo\n");
				char *erro = malloc(sizeof(int)*3);
					sprintf (erro, "%d", errno);
				packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno));
				send_pack(sock_client, packet);
				return;
			}
			printf("VAI ENTRAR WHILE>>>>>\n");
			packet = mount_pack(TYPE_PUT, _seq, (unsigned char*)file_name, strlen(file_name));
			send_pack(sock_client, packet);
			packet_recv = recv_pack(sock_client);
			if (packet_recv->t_e_p.byte_tp.type == TYPE_OK){
				pack_seq();
				while (fscanf(file, "%s", name) != EOF){
					if ((fp = fopen (name, "r")) == NULL) {
						printf ("GET: Não foi possivel abrir o arquivo\n");
						char *erro = malloc(sizeof(int)*3);
							sprintf (erro, "%d", errno);
						packet = mount_pack(TYPE_ERRO, _seq, (unsigned char *)erro, sizeof(errno));
						send_pack(sock_client, packet);
						return;
					}
					packet = mount_pack(TYPE_FILE_NAME, _seq, (unsigned char *)name, strlen(name));
					send_pack(sock_client, packet);
					packet_recv = recv_pack(sock_client);
					if (packet_recv->t_e_p.byte_tp.type == TYPE_OK){
						pack_seq();
						raw_file_send (sock_client, name, fp);
						printf("SAIU\n\n");
					}
					//free(name);
					//name = malloc(sizeof(char)*30);
					fclose(fp);
				}
			//free(name);
			packet = mount_pack(TYPE_END_GR, _seq, NULL, 0);
			send_pack(sock_client, packet);
			fclose(file);
			}
	}				

}

void raw_client_local_ls(char *option){

	FILE *ls;
	
	if(!strcmp(option, "list -l")){
		ls = popen("ls -l", "w");
	}
	else if (!strcmp(option, "list")){
		ls = popen("ls", "w");
	}

	pclose(ls);
	return;
}


void raw_client_local_cd(char *folder){
	
	char cwd[1024]; 
  		if ((chdir ((const char *) folder)) == 0) { //Tenta abrir o diretório
	    		getcwd (cwd, sizeof(cwd)); // Pega o nome do diretório corrente
			printf ("Entrou no diretório LOCAL: %s\n", cwd);
			return;
		}
		else{
			switch(errno){
			      case EACCES:{ 
					printf("Permissão Negada");
					return;
				}
			      case ENAMETOOLONG:{
					printf("O PATH é muito grande");
					return;
				}
			      case ENOTDIR:{
					printf("Um componente do PATH não é um diretório");
					return; 
				}
			      case ENOENT:{ 
					printf("O diretório não existe"); 
					return;
				}
			      default:{ 
					printf("Não foi possível mudar para o diretório %s", folder);
					return;
				}
			}
		}
			
}




