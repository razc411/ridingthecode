#include "server_defs.h"
#include "utils.h"

/*------------------------------------------------------------------------------------------------------------------
--      SOURCE FILE:    ChannelSystem.cpp -An application that will allow users to join chat channel
--                                      with asynchronous socket communcation using Select
--
--      PROGRAM:        client_chat
--
--      FUNCTIONS:
--                      void* ChannelManager(void * chdata)
--                      void process_incoming_message(int sock, C_MSG_PKT * client_msg, int c_num, int * channel_num, int * current_clients)
--                      void process_client_quit(int sock, C_QUIT_PKT * client_quit, int c_num, int * channel_num, int * current_clients)
--                      void process_add_client(int cm_pipe, int * max_fd, fd_set * listen_fds, int * channel_num, int * current_clients)

--      DATE:           March 14, 2014
--
--      REVISIONS:      (Date and Description)
--
--      DESIGNER:       Ramzi Chennafi
--
--      PROGRAMMER:     Ramzi Chennafi
--
--      NOTES:
--      The user can join chat channel by typing /join [channelname] and can start chatting with other
--      client who are in the same channel. They can also leave channel and join another one.
----------------------------------------------------------------------------------------------------------------------*/


extern int packet_sizes[];
static char client_names[MAX_CHANNELS][MAX_CLIENTS][MAX_USER_NAME];
static int socket_list[MAX_CHANNELS][MAX_CLIENTS];
static char channel_name[MAX_CHANNELS][MAX_CHANNEL_NAME];

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       ChannelManager
--
--      DATE:           March 14 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void* ChannelManager(void * chdata)
--                      void * chdata - data containing channel information
--
--      RETURNS:        void*
--
--      NOTES:
--      Takes care of control packet and message to the correct channel
----------------------------------------------------------------------------------------------------------------------*/
void* ChannelManager(void * chdata)
{
    int num_clients = 0; 
    int current_clients = 0;

    CHANNEL_DATA * cdata = (CHANNEL_DATA*) chdata;
	int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;
    int         type = -1;

    int channel_num = cdata->channel_num;
    max_fd = cdata->read_pipe;
    memcpy(channel_name[channel_num], cdata->channel_name, MAX_CHANNEL_NAME);
    printf("Channel %s opened.\n", channel_name[channel_num]);

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
                process_add_client(cdata->read_pipe, &max_fd, &listen_fds, &channel_num, &current_clients);
            }

            else if(type == CHANNEL_CLOSE)
            {
                for(int i = 0; i < num_clients; i++)
                {
                    close(socket_list[channel_num][i]);
                }
                break;
            }

            else if(type == USER_DISPLAY)
            {
                for(int i = 0; i < current_clients; i++)
                {
                    printf("%s\n", client_names[channel_num][i]);
                }
            }
    	}
        
        for(int i = 0; i < current_clients; i++)
        {
            if(FD_ISSET(socket_list[channel_num][i], &active))
            {
                void * packet = read_packet(socket_list[channel_num][i], &type);

                if(type == CLIENT_QUIT_PKT)
                {
                    FD_CLR(socket_list[channel_num][i], &listen_fds);
                    process_client_quit(socket_list[channel_num][i], (C_QUIT_PKT*)packet, i, 
                        &channel_num, &current_clients);
                }

                else if(type == CLIENT_MSG_PKT)
                {
                    process_incoming_message(socket_list[channel_num][i], (C_MSG_PKT*)packet, i, 
                        &channel_num, &current_clients);
                }
            }
        }    
    }

    pthread_exit(NULL);
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       process_incoming_message
--
--      DATE:           March 14 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void process_incoming_message(int sock, C_MSG_PKT * client_msg, int c_num, int * channel_num, int * current_clients)
--                      int sock - client socket
--                      C_MSG_PKT * client_msg - message packet from the client
--                      int c_num - client number
--                      int * channel_num - channel number
--                      int * current_clients - current number of clients
--
--      RETURNS:        void*
--
--      NOTES:
--      processes incoming message into the correct channel
----------------------------------------------------------------------------------------------------------------------*/
void process_incoming_message(int sock, C_MSG_PKT * client_msg, int c_num, int * channel_num, int * current_clients)
{
    S_MSG_PKT broadcast_msg;

    memcpy(broadcast_msg.client_name, client_names[*channel_num][c_num], MAX_USER_NAME);
    memcpy(broadcast_msg.msg, client_msg->msg, MAX_STRING);
    memcpy(broadcast_msg.channel_name, channel_name[*channel_num], MAX_CHANNEL_NAME);
        
    for(int i = 0; i < *current_clients; i++)
    {
        if(socket_list[*channel_num][i] != sock)
        {
            write_packet(socket_list[*channel_num][i], SERVER_MSG_PKT, &broadcast_msg);       
        }
    }

    printf("%s: %s: %s", channel_name[*channel_num], client_names[*channel_num][c_num], client_msg->msg);
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       process_client_quit
--
--      DATE:           March 14 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void process_client_quit(int sock, C_QUIT_PKT * client_quit, int c_num, int * channel_num, int * current_clients)
--                      int sock - client socket
--                      C_MSG_PKT * client_quit - quit control packet from the client
--                      int c_num - client number
--                      int * channel_num - channel number
--                      int * current_clients - current number of connected clients
--
--      RETURNS:        void
--
--      NOTES:
--      If user wants to quit, send quit message to the channel manager
----------------------------------------------------------------------------------------------------------------------*/
void process_client_quit(int sock, C_QUIT_PKT * client_quit, int c_num, int * channel_num, int * current_clients)
{
    if(client_quit->code == CLIENT_QUIT)
    {
        printf("Client %s left channel %s.\n", client_names[*channel_num][c_num], channel_name[*channel_num]);
    }

    memset(client_names[*channel_num][c_num], 0, MAX_USER_NAME);
    close(socket_list[*channel_num][c_num]);
    socket_list[*channel_num][c_num] = -1;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       process_add_client
--
--      DATE:           March 14 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void process_add_client(int cm_pipe, int * max_fd, fd_set * listen_fds, int * channel_num, int * current_clients)
--                      int cm_pipe - pipe for channel manager
--                      int * max_fd - pointer to the max file descriptor. Used for adding channel to select
--                      fd_set * listen_fds - pointer to the set file descriptors
--                      int * channel_num - channel number
--                      int * current_clients - current number of connected clients
--
--      RETURNS:        void
--
--      NOTES:
--      Adds the client to the channel
----------------------------------------------------------------------------------------------------------------------*/
void process_add_client(int cm_pipe, int * max_fd, fd_set * listen_fds, int * channel_num, int * current_clients)
{
    C_JOIN_PKT info_packet;
    S_CHANNEL_INFO_PKT s_info_pkt;

    read_pipe(cm_pipe, &info_packet, packet_sizes[CLIENT_JOIN_PKT]);

    for(int i = 0; i < *current_clients; i++)
    {
        if(strcmp(client_names[*channel_num][i], info_packet.client_name) == 0)
        {
            s_info_pkt.code = CONNECTION_REJECTED;
            write_packet(info_packet.tcp_socket, SERVER_MSG_PKT,(void*)&s_info_pkt);
            printf("Client %s exists, connection rejected.\n", info_packet.client_name);
            return;
        }
    }

    socket_list[*channel_num][*current_clients] = info_packet.tcp_socket;
    memcpy(client_names[*channel_num][*current_clients], info_packet.client_name, MAX_USER_NAME);
    
    *max_fd = *max_fd > socket_list[*channel_num][*current_clients] ? *max_fd : socket_list[*channel_num][*current_clients];
    FD_SET(socket_list[*channel_num][*current_clients], listen_fds);
    
    s_info_pkt.code = CONNECTION_ACCEPTED;
    s_info_pkt.num_clients = *current_clients;
    memcpy(s_info_pkt.channel_name, channel_name[*channel_num], MAX_CHANNEL_NAME);
    memcpy(s_info_pkt.channel_clients, client_names[*channel_num], MAX_USER_NAME);
    write_packet(info_packet.tcp_socket, CHANNEL_INFO_PKT,(void*)&s_info_pkt);

    *current_clients = *current_clients + 1;
    printf("Client %s added to channel %s.\n", info_packet.client_name, info_packet.channel_name);
}