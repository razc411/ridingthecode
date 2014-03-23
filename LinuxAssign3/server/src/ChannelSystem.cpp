#include "server_defs.h"
#include "utils.h"

extern int packet_sizes[];
static char client_names[MAX_CLIENTS][MAX_USER_NAME];
static int socket_list[MAX_CLIENTS];
static int num_clients = 0;
static int current_clients = 0;
static char channel_name[MAX_CHANNEL_NAME];
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: 
--
--      RETURNS: void
--
--      NOTES:
--      
----------------------------------------------------------------------------------------------------------------------*/
void* ChannelManager(void * chdata)
{
    CHANNEL_DATA * cdata = (CHANNEL_DATA*) chdata;
	int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;
    int         type = -1;

    max_fd = cdata->read_pipe;
    memcpy(channel_name, cdata->channel_name, MAX_CHANNEL_NAME);
    printf("Channel %s opened.\n", channel_name);

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
                process_add_client(cdata->read_pipe, &max_fd, &listen_fds);
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

                read_pipe(cdata->read_pipe, username, MAX_USER_NAME);
                read_pipe(cdata->read_pipe, msg, MAX_STRING);
                
                for(int i = 0; i < num_clients; i++)
                {
                    if(strcmp(client_names[i], username))
                    {
                        channel_client_kick(socket_list[i], i, msg);
                        break;
                    }
                }

            }
    	}
        
        for(int i = 0; i < num_clients; i++)
        {
            if(FD_ISSET(socket_list[i], &active))
            {
                void * packet = read_packet(socket_list[i], &type);

                if(type == JOIN_CHANNEL)
                {
                    write_packet(cdata->write_pipe, CLIENT_JOIN_PKT, packet);
                }

                if(type == CLIENT_QUIT)
                {
                    process_client_quit(socket_list[i], (C_QUIT_PKT*)packet, i);
                }

                else if(type == CLIENT_MSG_PKT)
                {
                    process_incoming_message(socket_list[i], (C_MSG_PKT*)packet, i);
                }
            }
        }

        if(num_clients != current_clients)
        {
            reform_lists();
        }
    
    }

    pthread_exit(NULL);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: 
--
--      RETURNS: void
--
--      NOTES:
--      
----------------------------------------------------------------------------------------------------------------------*/
void channel_client_kick(int sock, int client, char * msg)
{
    S_KICK_PKT kick_pkt;

    memcpy(kick_pkt.channel_name, channel_name, MAX_CHANNEL_NAME);
    memcpy(kick_pkt.msg, msg, MAX_STRING);

    write_packet(sock, CLIENT_KICK, (void*)&kick_pkt);

    printf("Client %s kicked from %s with msg %s.\n", client_names[client], channel_name, msg);
    memset(client_names[client], 0, MAX_USER_NAME);
    socket_list[client] = -1;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: 
--
--      RETURNS: void
--
--      NOTES:
--      
----------------------------------------------------------------------------------------------------------------------*/
void process_incoming_message(int sock, C_MSG_PKT * client_msg, int c_num)
{
    S_MSG_PKT broadcast_msg;

    memcpy(broadcast_msg.client_name, client_names[c_num], MAX_USER_NAME);
    memcpy(broadcast_msg.msg, client_msg->msg, MAX_STRING);
    memcpy(broadcast_msg.channel_name, channel_name, MAX_CHANNEL_NAME);
    
    for(int i = 0; i < current_clients; i++)
    {
       
            write_packet(socket_list[i], SERVER_MSG_PKT, &broadcast_msg);
        
    }

    printf("%s: %s: %s\n", channel_name, client_names[c_num], client_msg->msg);
    fflush(stdout);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: 
--
--      RETURNS: void
--
--      NOTES:
--      
----------------------------------------------------------------------------------------------------------------------*/
void process_client_quit(int sock, C_QUIT_PKT * client_quit, int c_num)
{
    socket_list[c_num] = -1;
    memset(client_names[c_num], 0, MAX_USER_NAME);

    if(client_quit->code == CLIENT_QUIT)
    {
        printf("Client %s left channel.\n", client_names[c_num]);
    }
    else if(client_quit->code == EXIT)
    {
        printf("Client %s shutdown.\n", client_names[c_num]);
    }

    current_clients--;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: 
--
--      RETURNS: void
--
--      NOTES:
--      
----------------------------------------------------------------------------------------------------------------------*/
void process_add_client(int cm_pipe, int * max_fd, fd_set * listen_fds)
{
    C_JOIN_PKT info_packet;
    S_CHANNEL_INFO_PKT s_info_pkt;

    read_pipe(cm_pipe, &info_packet, packet_sizes[CLIENT_JOIN_PKT]);

    for(int i = 0; i < current_clients; i++)
    {
        if(strcmp(client_names[i], info_packet.client_name) == 0)
        {
            s_info_pkt.code = CONNECTION_REJECTED;
            write_packet(info_packet.tcp_socket, SERVER_MSG_PKT,(void*)&s_info_pkt);
            printf("Client %s exists, connection rejected.\n", info_packet.client_name);
            return;
        }
    }

    socket_list[current_clients] = info_packet.tcp_socket;
    memcpy(client_names[current_clients], info_packet.client_name, MAX_USER_NAME);
    
    *max_fd = *max_fd > socket_list[current_clients] ? *max_fd : socket_list[current_clients];
    FD_SET(socket_list[current_clients], listen_fds);
    
    num_clients++;
    s_info_pkt.code = CONNECTION_ACCEPTED;
    s_info_pkt.num_clients = num_clients;
    memcpy(s_info_pkt.channel_name, channel_name, MAX_CHANNEL_NAME);
    memcpy(s_info_pkt.channel_clients, client_names, MAX_USER_NAME);
    write_packet(info_packet.tcp_socket, CHANNEL_INFO_PKT,(void*)&s_info_pkt);

    current_clients++;
    printf("Client %s added to channel %s.\n", info_packet.client_name, info_packet.channel_name);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: 
--
--      RETURNS: void
--
--      NOTES:
--      
----------------------------------------------------------------------------------------------------------------------*/
void reform_lists()
{
    char * temp_client_names[MAX_CLIENTS];
    int temp_socket_list[MAX_CLIENTS];
    int j = 0;
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(client_names[i] != NULL)
        {
            temp_client_names[j] = client_names[i];
            temp_socket_list[j++] = socket_list[i];
        }
    }
    memcpy(client_names, temp_client_names, (sizeof(char) * MAX_USER_NAME) * MAX_CLIENTS);
    memcpy(socket_list, temp_socket_list, sizeof(int) * MAX_CLIENTS);
    num_clients = current_clients;
}