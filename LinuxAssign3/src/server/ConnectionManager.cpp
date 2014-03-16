#include "ConnectionManager.h"
#include "utils.h"
#include "ServerSystemRouter.h"
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void init_client(HWND hwnd) , takes the parent HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      Intializes the client, the type of intialization depends on the chosen protocol in the settings. Binds whenever
--		the connection is TCP.
----------------------------------------------------------------------------------------------------------------------*/
void* ConnectionManager(void * cmdata)
{
	IPaddress ipaddr;
	C_JOIN_PKT * info_packet;
	uint32_t type = CLIENT_ADD;
	CMANAGERDATA * cdata = (CMANAGERDATA*) cmdata;

	resolve_host(&ipaddr, TCP_PORT, INADDR_ANY);

	TCPsocket listeningsock = SDLNet_TCP_Open(&ipaddr);
	if(!listeningsock)
	{
		fprintf(stderr, "Error creating accept socket : %s ", SDLNet_GetError());
		exit(2);
	}

	while(1)
	{
		TCPsocket client_tcpsock = NULL;
		
		client_tcpsock = SDLNet_TCP_Accept(listeningsock);
		if(client_tcpsock != NULL)
		{
			info_packet = (C_JOIN_PKT*)recv_tcp_packet(client_tcpsock, &type);

			write_pipe(cdata->write_pipe, &type, TYPE_SIZE);
			write_pipe(cdata->write_pipe, info_packet, sizeof(C_JOIN_PKT));
			printf("Client %s accepted.", info_packet->client_name);
		}
	}

	return NULL;
}	