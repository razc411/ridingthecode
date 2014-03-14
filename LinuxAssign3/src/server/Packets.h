#ifndef PACKETS_H
#define PACKETS_H

//INTERNAL SERVER PKT DEFS
#define CLIENT_KICK  	0
#define CLIENT_ADD   	1
#define CLIENT_QUIT	 	2
#define CHANNEL_EXIT 	3
#define CHANNEL_CREATE 	4
#define CHANNEL_CLOSE 	5

//INTERNAL CLIENT PKT DEFS 
#define JOIN_CHANNEL	0
#define QUIT_CHANNEL    1
#define EXIT 			2
#define CLIENT_MSG    	3

//CONNECTION CODES
#define REJECTED_CONNECTION -1
#define CONNECTION_ACCEPTED  1
#define CLIENT_LIST_UPDATE	 0

//SERVER SIDE OUTGOING PKTS
#define SERVER_KICK_PKT		0
#define SERVER_MSG_PKT		1
#define CHANNEL_INFO_PKT 	2

//CLIENT SIDE OUTGOING PKTS
#define CLIENT_MSG_PKT		3
#define CLIENT_QUIT_PKT		4
#define CLIENT_JOIN_PKT		5

// OUTGOING PACKET DEFS
typedef struct pkt0
{	
	char * channel_name;
	char * kick;
} S_KICK_PKT;

typedef struct pkt1
{
	char * client_name;
	char * msg;
	char * channel_name;
} S_MSG_PKT;

typedef struct pkt2
{
	int code;
	char * channel_name;
	char ** channel_clients;
} S_CHANNEL_INFO_PKT;

typedef struct pkt3
{
	char * client_name;
	char * msg;
} C_MSG_PKT;

typedef struct pkt4
{
	int code;
} C_QUIT_PKT;

typedef struct pkt5{
	char * client_name;
	char * channel_name;
} C_JOIN_PKT;

#endif

