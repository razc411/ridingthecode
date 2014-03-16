#include "server_defs.h"
#include "utils.h"

char ** client_name = (char**) malloc((sizeof(char) * MAX_CLIENTS) * MAX_USER_NAME);
int * socket_list = (int*) malloc(sizeof(int) * MAX_CLIENTS);
static int num_clients = 0;
static char channel_name[MAX_CHANNEL_NAME];

void* ChannelManager(void * chdata)
{
    CHANNEL_DATA * cdata = (CHANNEL_DATA*) chdata;
	int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;
    uint32_t    type = -1;

    max_fd = cdata->read_pipe;

	FD_ZERO(&listen_fds);
    FD_SET(cdata->read_pipe, &listen_fds);

    while(1)
    {
        active = listen_fds;
    	select(max_fd + 1, &active, NULL, NULL, NULL);

    	if(FD_ISSET(cdata->read_pipe, &active))
    	{
            if(read_pipe(cdata->read_pipe, &type, TYPE_SIZE) == -1){}

            else if(type == CLIENT_ADD)
            {
                add_client(cdata->read_pipe);
            }

            else if(type == CHANNEL_CLOSE)
            {
                //broadcast channel close message
                //close all connections
            }

            else if(type == CLIENT_KICK)
            {
                //send client kick message
                //remove client from list
                //close connection
            }
    	}  
    
    }

    return NULL;
}

void new_channel_client(int rd_pipe)
{
    C_JOIN_PKT * info_packet = (C_JOIN_PKT*)malloc(sizeof(C_JOIN_PKT));

    read_pipe(rd_pipe, (void*)info_packet, sizeof(C_JOIN_PKT));
    socket_list[num_clients] = info_packet->tcp_socket;
    memcpy(info_packet->client_name, client_name[num_clients], sizeof(MAX_USER_NAME));

    num_clients++;
    printf("Client %s added to channel %s.\n", info_packet->client_name, info_packet->channel_name);
}