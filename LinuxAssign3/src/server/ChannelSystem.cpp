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
                for(int i = 0; i < num_clients; i++)
                {
                    close(socket_list[i]);
                }
                break;
            }

            else if(type == CLIENT_KICK)
            {
                char username[MAX_USER_NAME];
                char msg[MAX_STRING];

                read_pipe(cdata->read_pipe, username, MAX_USER_NAME) == -1)
                read_pipe(cdata->read_pipe, msg, MAX_STRING) == -1)
                
                for(int i = 0; i < num_clients; i++)
                {
                    if(strcmp(client_name[i], username))
                    {
                        client_kick(socket_list[i], i, msg);
                    }
                }
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

void channel_client_kick(int sock, int client, char * msg)
{
    S_KICK_PKT kick_pkt;
    uint32_t type = CLIENT_KICK;

    kick_pkt.channel_name = channel_name;
    kick_pkt.msg = msg;
    client_name[client] = NULL;

    write(sock, &type, TYPE_SIZE);
    write(sock, &kick_pkt, sizeof(S_KICK_PKT));
}