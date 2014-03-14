#include "ConnectionManager.h"


void* ConnectionManager(void * cmdata)
{
	IPAddress ipaddr;
	resolve_host(&ipaddr, TCP_PORT, INADDR_ANY);
	SDLNet_TCP_Open(&ipaddr);

}