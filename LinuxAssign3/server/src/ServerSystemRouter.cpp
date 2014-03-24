#include "server_defs.h"
#include "utils.h"
/*------------------------------------------------------------------------------------------------------------------
--      SOURCE FILE:    ServerSystemRouter.cpp -An application that will allow users to join chat channel
--                                      with asynchronous socket communcation using Select
--
--      PROGRAM:        client_chat
--
--      FUNCTIONS:
--                      int main()
--                      void add_client(int client_sd, void * join_req, int joined)void reject_client(int client_sd, C_JOIN_PKT * info_pkt)
--                      void add_channel(int * max_fd, fd_set * listen_fds, int input_pipe)
--                      void channel_close(int input_pipe)
--                      void close_server(int input_pipes[2])
--                      void display_users(int input_pipe)
--                      void* InputManager(void * indata)
--
--      DATE:           March 7, 2014
--
--      REVISIONS:      (Date and Description)
--
--      DESIGNER:       Ramzi Chennafi
--
--      PROGRAMMER:     Ramzi Chennafi
--
--      NOTES:
--      The server side of the linux chat server. A user can create channels on the server and clients can join these channels.
--      The server can handle multiple channels and users on different channels at the same time, and broadcast messages between
--      them. 
--      COMMANDS
--      /create <channel name> - creates a new channel
--      /users <channel name> - displays users in a channel
--      /close <channel name> - closes the sepcified channel
--      /exit - exits the server and closes all connections
----------------------------------------------------------------------------------------------------------------------*/
extern int          packet_sizes[];
static int          open_channels = 0;
static char         channel_name_list[MAX_CHANNELS][MAX_CHANNEL_NAME];
static int          channel_pipes[MAX_CHANNELS][2];
static pthread_t    thread_channel[MAX_CHANNELS];
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       main
--
--      DATE:           March 8 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      int main()
--
--      RETURNS:        int
--
--      NOTES:
--      Main hub of the server. Handles all channel and input management. Also listens for connecting users.
----------------------------------------------------------------------------------------------------------------------*/
int main()
{
    pthread_t 	thread_input;

    int         input_pipe[2];
  
    int 		max_fd, client_sd;
    fd_set      listen_fds;
    fd_set      active;

    int    type = -1;

    THREAD_DATA * idata = (THREAD_DATA*)malloc(sizeof(THREAD_DATA));

	pipe(input_pipe);

    idata->write_pipe = input_pipe[WRITE];
    idata->read_pipe = input_pipe[READ];

    dispatch_thread(InputManager, (void*)idata, &thread_input);

    int listen_sd = create_accept_socket();
    
    max_fd = input_pipe[READ] > listen_sd ? input_pipe[READ] : listen_sd;

	FD_ZERO(&listen_fds);
    FD_SET(input_pipe[READ], &listen_fds);
    FD_SET(listen_sd, &listen_fds);

    while(1)
    {
        int sel_ret;
        active = listen_fds;
    	sel_ret = select(max_fd + 1, &active, NULL, NULL, NULL);
        
        if(sel_ret == -1)
        {
            perror("select");
            continue;
        }

        if(FD_ISSET(listen_sd, &active))
        {
            client_sd = accept_new_client(listen_sd);
            if(client_sd != -1)
                add_client(client_sd, NULL, 0);
        }

    	if(FD_ISSET(input_pipe[READ], &active))
    	{  
            
            if(read_pipe(input_pipe[READ], &type, TYPE_SIZE) == -1){}

            else if(type == CHANNEL_CREATE)
    		{
    			add_channel(&max_fd, &listen_fds, input_pipe[READ]);
    		}

    		else if(type == CHANNEL_CLOSE)
            {
                channel_close(input_pipe[READ]);
            }

            else if(type == USER_DISPLAY)
            {
                display_users(input_pipe[READ]);
            }

    		else if(type == SERVER_EXIT)
            {
                close_server(input_pipe);
                break;
            }
    	}
    }

    free(idata);
    close(input_pipe[READ]);
    close(input_pipe[WRITE]);

    return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: add_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void add_client(int client_sd)
--                  int client sd - socket of the requesting client
--
--      RETURNS: void
--
--      NOTES:
--      Called whenever a client connects to a server and requests to join a channel.
--      Will retrieve the C_JOIN_PKT from the client, and redirect the client to the proper
--      channel.
----------------------------------------------------------------------------------------------------------------------*/
void add_client(int client_sd, void * join_req, int joined)
{
    C_JOIN_PKT * info_packet;
    bool valid = false;
    int type = -1;
        
    info_packet = (C_JOIN_PKT*)read_packet(client_sd, &type);
    if(type != CLIENT_JOIN_PKT)
    {
        perror("Failed to add client at read_packet");
        return;
    }

    info_packet->tcp_socket = client_sd;

    type = CLIENT_ADD;

    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], info_packet->channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            write_pipe(channel_pipes[i][WRITE], info_packet, sizeof(C_JOIN_PKT));
            valid = true;
            break;
        }
    }
    
    if(valid == false)
    {
        reject_client(client_sd, info_packet);
        return;
    }

    printf("Client passed to channel for adding.\n");
}

void reject_client(int client_sd, C_JOIN_PKT * info_pkt)
{
    S_CHANNEL_INFO_PKT pkt;
    pkt.code = CONNECTION_REJECTED;
    write_packet(client_sd, CHANNEL_INFO_PKT, &pkt);

    printf("Channel does not exist, client %s rejected for channel %s.\n", 
            info_pkt->client_name, info_pkt->channel_name);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       add_channel
--
--      DATE:           March 15 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void add_channel(CHANNEL_DATA * chdata, int * max_fd, fd_set * listen_fds, int input_pipe)
--                      int * max_fd - pointer to the max file descriptor. Used for adding channel to select.
--                      fd_set * listen_fds - pointer to the set file descriptors.
--                      int input_pipe - file descriptor to the input manager pipe
--
--      RETURNS:        void
--
--      NOTES:
--      Establishes a new channel thread and adds it to the router select system.  
----------------------------------------------------------------------------------------------------------------------*/
void add_channel(int * max_fd, fd_set * listen_fds, int input_pipe)
{
    char temp[MAX_CHANNEL_NAME];
    CHANNEL_DATA * chdata = (CHANNEL_DATA*)malloc(sizeof(CHANNEL_DATA));

    read_pipe(input_pipe, temp, MAX_CHANNEL_NAME);
    memcpy(channel_name_list[open_channels], temp, MAX_CHANNEL_NAME);
    chdata->channel_num = open_channels;
    chdata->channel_name = temp;

	pipe(channel_pipes[open_channels]);
	chdata->write_pipe = channel_pipes[open_channels][WRITE];
	chdata->read_pipe = channel_pipes[open_channels][READ];

    *max_fd = *max_fd > channel_pipes[open_channels][READ] ? *max_fd : channel_pipes[open_channels][READ];
    FD_SET(channel_pipes[open_channels][READ], listen_fds);

	dispatch_thread(ChannelManager, (void*)chdata, &thread_channel[open_channels]);
    pthread_join(thread_channel[open_channels++], NULL);
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       channel_close
--
--      DATE:           March 15 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void channel_close(int input_pipe)
--                      int input_pipe - file descriptor to the input manager pipe
--
--      RETURNS:        void
--
--      NOTES:
--      Passes the close channel message to the appropriate channel. 
----------------------------------------------------------------------------------------------------------------------*/
void channel_close(int input_pipe)
{
    uint32_t type = CHANNEL_CLOSE;
    char channel_name[MAX_CHANNEL_NAME];

    read_pipe(input_pipe, channel_name, MAX_CHANNEL_NAME);
    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            memset(channel_name_list[i], 0, MAX_CHANNEL_NAME);
            break;
        }
    }  

    printf("Channel %s closed.\n", channel_name);
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       close_server
--
--      DATE:           March 15 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void close_server(int input_pipes[2])
--                      int input_pipe[2] - pipe to handle input messages
--
--      RETURNS:        void
--
--      NOTES:
--      Closes all open channels and prints out exit message.
----------------------------------------------------------------------------------------------------------------------*/
void close_server(int input_pipes[2])
{
    uint32_t type = CHANNEL_CLOSE;
    for(int i = 0; i < open_channels; i++)
    {
        write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
        printf("Channel %s closed.\n", channel_name_list[i]);
    }

    printf("Server exiting.\n");
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       display_users
--
--      DATE:           March 15 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void close_server(int input_pipes[2])
--                      int input_pipe[2] - pipe to handle input messages
--
--      RETURNS:        void
--
--      NOTES:
--      Sends a USER_DISPLAY message to the proper channel. The result will be all users in that
--      channel being printed out.
----------------------------------------------------------------------------------------------------------------------*/
void display_users(int input_pipe)
{
    char channel_name[MAX_CHANNEL_NAME];
    int type = USER_DISPLAY;
    read_pipe(input_pipe, channel_name, MAX_CHANNEL_NAME);

    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            break;
        }
    }  
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       InputManager
--
--      DATE:           March 8 2014
--      REVISIONS:      none
--
--      DESIGNER:       Ramzi Chennafi
--      PROGRAMMER:     Ramzi Chennafi
--
--      INTERFACE:      void* InputManager(void * indata)
--                      void * indata - incoming data from client
--
--      RETURNS:        void*
--
--      NOTES:
--      Takes input from the terminal and responds accordingly. Will listen for the /users /create /close /exit and 
--      plain message commands. Will always send its responses to main() for proper routing.
----------------------------------------------------------------------------------------------------------------------*/
void* InputManager(void * indata)
{
    THREAD_DATA * input_data = (THREAD_DATA*) indata;

    uint32_t type;
    char *   temp = NULL;
    char     cmd[MAX_STRING];
    char     channelname[MAX_CHANNEL_NAME];
    size_t   nbytes = MAX_STRING;

    while(getline(&temp, &nbytes, stdin))
    {
        sscanf(temp, "%s", cmd);
        
        if(strcmp(cmd, "/create") == 0)
        {
            type = CHANNEL_CREATE;
            if(sscanf(temp, "%s %s", cmd, channelname) == 2)
            {
                write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
                write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
            }
            else
            {
                printf("Improper syntax, expected: /create <channel name>, channel must be one word.\n");
            }
        }
        else if(strcmp(cmd, "/exit") == 0)
        {
            type = SERVER_EXIT;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            break;
        }
        else if(strcmp(cmd, "/users") == 0)
        {
            type = USER_DISPLAY;
            if(sscanf(temp, "%s %s", cmd, channelname) == 2)
            {
                write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
                write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
            }
            else
            {
                printf("Improper syntax, expected: /users <channel name>.\n");
            }
        }
        else if(strcmp(cmd, "/close") == 0)
        {
            type = CHANNEL_CLOSE;
            if(sscanf(temp, "%s %s", cmd, channelname) == 2)
            {
                write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
                write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
            }
            else
            {
                printf("Improper syntax, expected: /close <channel name>\n");
            }
        }
    }

    return NULL;
}