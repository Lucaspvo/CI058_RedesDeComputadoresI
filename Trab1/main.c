#include "main.h"



int main(int argc, char **argv)
{
		
		if (argc < 2){
			_help(1);
			return 0;
		}
		
		else if (strcmp (argv[1], "-c") == 0){
			printf("Modo cliente ativado\n");
			raw_client_init();
		}
		else if (strcmp (argv[1], "-s") ==0){
			printf("Modo Servidor ativado\n");
			raw_server_init();
		}
		else{
			_help(2);
		}
	return 0;

}

