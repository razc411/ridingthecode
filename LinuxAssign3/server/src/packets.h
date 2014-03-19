#ifndef PACKETS_H
#define PACKETS_H

#define MAX_ARGS 			4
#define MAX_CLIENTS 		100
#define MAX_STRING 			1024
#define TCP_PORT 			4333
#define MAX_CHANNELS 		50
#define READ 				0
#define WRITE 				1
#define MAX_CHANNEL_NAME 	20
#define MAX_USER_NAME		20
#define TYPE_SIZE			sizeof(uint32_t)

//INTERNAL SERVER PKT DEFS
#define CLIENT_KICK  	0
#define CLIENT_ADD   	1
#define CLIENT_QUIT	 	2
#define SERVER_EXIT 	3
#define CHANNEL_CREATE 	4
#define CHANNEL_CLOSE 	5

//INTERNAL CLIENT PKT DEFS 
#define JOIN_CHANNEL	0
#define QUIT_CHANNEL    1
#define EXIT 			2
#define CLIENT_MSG    	3

//CONNECTION CODES
#define CONNECTION_REJECTED -1
#define CONNECTION_ACCEPTED  1
#define CLIENT_LIST_UPDATE	 0

//SERVER SIDE OUTGOING PKTS
#define SERVER_KICK_PKT		0
#define SERVER_MSG_PKT		1
#define CHANNEL_INFO_PKT 	2
#define CHANNEL_ERROR		3

//CLIENT SIDE OUTGOING PKTS
#define CLIENT_MSG_PKT		4
#define CLIENT_QUIT_PKT		5
#define CLIENT_JOIN_PKT		6
#define CLIENT_ERROR		7

// OUTGOING PACKET DEFS
typedef struct pkt0
{	
	char * client_name;
	char * channel_name;
	char * msg;
} S_KICK_PKT;

typedef struct pkt1
{
	char * client_name;
	char * msg;
	char * channel_name;
} S_MSG_PKT;

typedef struct pkt2
{
	uint32_t code;
	char * channel_name;
	char ** channel_clients;
} S_CHANNEL_INFO_PKT;

typedef struct pkt3
{
	char * msg;
} C_MSG_PKT;

typedef struct pkt4
{
	int code;
} C_QUIT_PKT;

typedef struct pkt5{
	char * client_name;
	char * channel_name;
	int tcp_socket;
} C_JOIN_PKT;

#endif

