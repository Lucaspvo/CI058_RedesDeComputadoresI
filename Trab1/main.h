#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <arpa/inet.h>
#include <linux/if_ether.h>
#include <linux/if.h>
#include <netinet/in.h>
#include <netpacket/packet.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <poll.h>
#include <sys/statvfs.h>


//--- Configurações do Socket devem ser alteradas dependendo da maquina.

#define SOCKET_PORT "7777"
#define SOCKET_CLIENT_DEVICE "eth0"//casa
//#define SOCKET_CLIENT_DEVICE "eth0" Uni
#define SOCKET_SERVER_DEVICE "eth0"


//--- TABELA DE TIPOS --- Utilizada no pacote.
//------------------------
//   TYPE    ||     CODE |
//------------------------   
#define TYPE_ACK     0
#define TYPE_NACK    1
#define TYPE_OK      2
#define TYPE_FILE_NAME 3
#define TYPE_DIR_NAME 4
#define TYPE_TAM_ARQ 5
#define TYPE_DATA    6
#define TYPE_SHOW    8
#define TYPE_END_GR  9
#define TYPE_LS      10
#define TYPE_PUT     11
#define TYPE_CD      12
#define TYPE_GET     13
#define TYPE_ERRO    14
#define TYPE_END_U   15
//-----------------------


//--- TABELA DE ERROS ---- Utilizada para enviar um erro pelo pacote.
//-------------------------
//   ERROR    ||     CODE |
//-------------------------   
#define E_PERMISSION  '0'
#define E_DIR_INEX    '1' 
#define E_INSUF_SIZE  '2'
#define E_ARQ_INEX    '3'
//-----------------------


//Packet definition! 
#define PACK_INIT       126     //Fixado em 01111110 = 126 decimal
#define PACK_SIZE_MIN 	3         //tamanho minimo por causa dos campos seq, tipo e paridade. 
#define PACK_SIZE_MAX 	66                // tamanho maximo do pacote.
#define PACK_SEQ        4                       // Número de pacotes numa sequencia a qual se chegar a 4 deve retornar para 0.
#define PACK_DATA       63              // Tamanho do arquivo, pois o size_max engloba os campos PACK_SEQ, PACK_TYPE, PACK_PARITY.
#define PACK_TIMEOUT    10000    //Tempo para receber o próximo pacote
#define PACK_TRIES      6


// Byte paridade e tipo
typedef struct par_tipo{
		unsigned char type:4;
		unsigned char parity:4;
}par_tipo;

union parity_type{
	unsigned char value;
	par_tipo byte_tp;
};

//Byte size e seq
typedef struct tam_seq{
	unsigned char size:6;
    unsigned char seq:2;
}tam_seq;

union size_seq{
	unsigned char value;
	tam_seq byte_ss;
};



typedef struct _pack{
        unsigned char init:8;
        union size_seq s_e_s;
        //unsigned char size:6;
        //unsigned char seq:2;
        //unsigned char type;         //########## Arrumar Tipos, colocar size + seq em um byte, e type + parity em outro com uma union
        //unsigned char parity:4;
		union parity_type t_e_p;
        unsigned char *data;
} *pack;                //Estrutura padrão do pacote



//Usado para fazer a paridade
typedef struct byte_data{
	unsigned char four_first:4;
	unsigned char four_last:4;	
} byte_data;

union byte{
	unsigned char value;
	byte_data byte;
};


extern int _seq;   

void _help();           //Função de ajuda presente em raw_help.c

int create_sock(const char *);  //Função utilizada para criar o socket tanto do cliente quanto do servidor, presente em raw_socket.c


//----- Funções do cliente presentes em raw_client.c

void raw_client_init();
void raw_client_cd(int, char *);
void raw_client_ls(int, char *);
void raw_client_get(int, char *);
void raw_client_put(int, char *);
void raw_client_local_ls(char *);
void raw_client_local_cd(char *);
void raw_file_recv (int, char *, FILE *);
void raw_file_send (int, char *, FILE *);

//----- Funções do servidor presentes em raw_server.c

void raw_server_init();
void raw_server_cd(int, unsigned char *);
void raw_server_ls(int, unsigned char * );
void raw_server_put(int, unsigned char *);
void raw_server_get(int, unsigned char *);
void raw_send_ls (int , FILE *);
//Funções Pacotes;

pack mount_pack(int , int , unsigned char *, int); //Função para montar o pacote.
int parity_pack(pack); //Paridade do pacote.
pack string_to_packet(unsigned char *);
unsigned char* packet_to_string(pack);
int send_pack(int, pack);
pack recv_pack(int);
void pack_seq ();
int raw_pack_timeout(int, int); // sock e offset do timeout

void anti_pack_seq(pack);

