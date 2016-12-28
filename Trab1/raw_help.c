#include "main.h"

void _help (int erro) {
	if (erro == 1)
		printf("modo de uso: ./rawsock [ -c (modo cliente) ou -s (modo servidor) ]\n");
	else if (erro == 2){
		printf("Parametro errado\n");
		_help(1);
	}
}

