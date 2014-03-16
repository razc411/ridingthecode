#include "ChannelSystem.h"

char ** client_name = (char**) malloc((sizeof(char) * MAX_CLIENTS) * MAX_USER_NAME);
TCPsocket * socket_list = (TCPsocket*) malloc(sizeof(TCPsocket) * MAX_CLIENTS);
static int num_clients = 0;

void* ChannelManager(void * chdata)
{
    CHANNEL_DATA * cdata = (CHANNEL_DATA*) chdata;
	int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    max_fd = cdata->read_pipe;

	FD_ZERO(&listen_fds);
    FD_SET(cdata->read_pipe, &listen_fds);

    type = -1;

    while(1)
    {
    	int ret;
        active = listen_fds;
    	ret = select(max_fd + 1, &active, NULL, NULL, NULL);

    	if(ret && FD_ISSET(cdata->read_pipe, &active))
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
}

void add_client(int read_pipe)
{
    C_JOIN_PKT info_packet;

    read_pipe(cm_pipe, &info_packet, sizeof(C_JOIN_PKT));

    num_clients++;
    printf("Client passed to channel for adding.\n");
}