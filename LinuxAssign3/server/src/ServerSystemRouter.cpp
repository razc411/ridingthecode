#include "server_defs.h"
#include "utils.h"
/*------------------------------------------------------------------------------------------------------------------
--      PROJECT: 
--      FUNCTION: main()
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int main()
--
--      RETURNS: void
--
--      NOTES:
--      Intializes the client, the type of8 intialization depends on the chosen protocol in the settings. Binds whenever
--      the connection is TCP.
----------------------------------------------------------------------------------------------------------------------*/
extern int packet_sizes[];
int last_channel 		= 0;
int current_channels    = 0;
int open_channels 		= 0;
int num_clients 		= 0;
char **channel_name_list = (char**) malloc(sizeof(char*) * MAX_CHANNELS);	
static int channel_pipes[MAX_CHANNELS][2];
static pthread_t   thread_channel[MAX_CHANNELS];
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main()
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int
    C_JOIN_PKT * info_packet = (C_JOIN_PKT*)malloc(sizeof(C_JOIN_PKT));

    if(read_packet(client_sd, (char*)info_packet) != CLIENT_ADD)
    { main()
--
--      RETURNS: void
--
--      NOTES:
--      Intializes the server. Does the following actions as part of its intialization.
--      - Creates an input thread and an input pipe
--      - Creates a listening socket
--      - Waits on a select statement, listening to the input thread and the channel threads.
--      Works as the central hub for all channels and the input thread. Redirects commands
--      from the input thread to the proper channel threads using IPC.
----------50eter
class client_utils{

    private:
    //The storage string
    std::string str;

    //The text surface
    SDL_Surface *text;------------------------------------------------------------------------------------------------------------*/
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
    		
            else if(type == CLIENT_KICK)
            {
                kick_client(input_pipe[READ]);
            }

    		else if(type == CHANNEL_CLOSE)
            {
                channel_close(input_pipe[READ]);
            }

    		else if(type == SERVER_EXIT)
            {
                close_server(input_pipe);
                break;
            }
    	}

    	for(int i = 0; i < open_channels; i++)
    	{
    		if(FD_ISSET(channel_pipes[i][READ], &active))
    		{
    			void * packet = read_packet(channel_pipes[i][READ], &type);

                if(type == JOIN_CHANNEL)
                {
                    int res = CONNECTION_ACCEPTED;
                    write(((C_JOIN_PKT*)packet)->tcp_socket, &res, TYPE_SIZE);
                    client_sd = accept_new_client(listen_sd);
                    if(client_sd != -1)
                        add_client(client_sd, packet, 1);
                }   
    		}
    	}

        //if(open_channels != current_channels)
       // {
       //     reform_lists();
        //}
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
    int type = -1;

    if(joined == 0)
    {
        
        info_packet = (C_JOIN_PKT*)read_packet(client_sd, &type);
        if(type != CLIENT_JOIN_PKT)
        {
            perror("Failed to add client at read_packet");
            return;
        }

        info_packet->tcp_socket = client_sd;
    }
    else
    {
        info_packet = (C_JOIN_PKT*)join_req;
        info_packet->tcp_socket = client_sd;
    }

    type = CLIENT_ADD;

    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], info_packet->channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            write_pipe(channel_pipes[i][WRITE], info_packet, sizeof(C_JOIN_PKT));
            break;
        }
    }
    
    num_clients++;
    printf("Client passed to channel for adding.\n");
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: add_channel
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void add_channel(CHANNEL_DATA * chdata, int * max_fd, fd_set * listen_fds, int input_pipe)
--                  int * max_fd - pointer to the max file descriptor. Used for adding channel to select.
--                  fd_set * listen_fds - pointer to the set file descriptors.
--                  int input_pipe - file descriptor to the input manager pipe
--
--      RETURNS: void
--
--      NOTES:
--      Establishes a new channel thread and adds it to the router select system.
----------------------------------------------------------------------------------------------------------------------*/
void add_channel(int * max_fd, fd_set * listen_fds, int input_pipe)
{
    char temp[MAX_CHANNEL_NAME];
    CHANNEL_DATA * chdata = (CHANNEL_DATA*)malloc(sizeof(CHANNEL_DATA));

    read_pipe(input_pipe, &temp, MAX_CHANNEL_NAME);
    channel_name_list[open_channels] = (char*)malloc(sizeof(char) * MAX_CHANNEL_NAME);
    memcpy(channel_name_list[open_channels], temp, MAX_CHANNEL_NAME);
    chdata->channel_num = open_channels;
    chdata->channel_name = temp;

	pipe(channel_pipes[open_channels]);
	chdata->write_pipe = channel_pipes[open_channels][WRITE];
	chdata->read_pipe = channel_pipes[open_channels][READ];

	if(open_channels > last_channel)
    {
    	last_channel = open_channels;
        *max_fd = *max_fd > channel_pipes[last_channel][READ] ? *max_fd : channel_pipes[last_channel][READ];
        FD_SET(channel_pipes[last_channel][READ], listen_fds);
	}


	dispatch_thread(ChannelManager, (void*)chdata, &thread_channel[open_channels]);
    pthread_join(thread_channel[open_channels++], NULL);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: kick_client
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void kick_client(int input_pipe)
--                  int input_pipe - file descriptor to the input manager pipe
--      RETURNS: void
--
--      NOTES:
--      Passes the kick message to the apropriate channel.
----------------------------------------------------------------------------------------------------------------------*/
void kick_client(int input_pipe)
{
    S_KICK_PKT s_kick;

    read_pipe(input_pipe, &s_kick, packet_sizes[CLIENT_KICK]);
        
    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], s_kick.channel_name) == 0)
        {
            write_packet(channel_pipes[i][WRITE], CLIENT_KICK, &s_kick);
            printf("Client kick msg passed to channel %s.\n", channel_name_list[i]);
            break;
        }
    }
        
    --num_clients;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: channel_close
--
--      DATE: March 15 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void channel_close(int input_pipe)
--
--      RETURNS: void
--
--      NOTES:
--      Passes the close channel message to the appropriate channel.
----------------------------------------------------------------------------------------------------------------------*/
void channel_close(int input_pipe)
{
    uint32_t type = CHANNEL_CLOSE;
    char * channel_name = NULL;

    read_pipe(input_pipe, &channel_name, MAX_CHANNEL_NAME);
    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            channel_name_list[i] = NULL;
            break;
        }
    }  

    printf("Channel %s closed.\n", channel_name);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void close_server(int input_pipe[2])
--                  int input_pipe[2] - pipe to the input manager
--      RETURNS: void
--
--      NOTES:
--      Closes all open pipes and broadcasts an exit message to the channels.
--      Server will exit after all channels have finished their closing operations.
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
--      the connection is TCP.
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
            sscanf(temp, "%s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
        else if(strcmp(cmd, "/kick") == 0)
        {
            S_KICK_PKT s_kick;
            sscanf(temp, "%s %s %s %s", cmd, s_kick.channel_name, s_kick.client_name, s_kick.msg);
            write_packet(input_data->write_pipe, CLIENT_KICK, &s_kick);
        }
        else if(strcmp(cmd, "/exit") == 0)
        {
            type = SERVER_EXIT;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            break;
        }
        else if(strcmp(cmd, "/close") == 0)
        {
            type = CHANNEL_CLOSE;
            sscanf(temp, "%s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
    }

    return NULL;
}

void reform_router_lists()
{
    char * temp_channel_names[MAX_CHANNELS];

    int j = 0;
    for(int i = 0; i < MAX_CLIENTS; i++)
    {
        if(channel_name_list[i] != NULL)
        {
            temp_channel_names[j++] = channel_name_list[i];
        }
    }

    memcpy(channel_name_list, temp_channel_names, (sizeof(char) * MAX_CHANNEL_NAME) * MAX_CHANNELS);
    current_channels = open_channels;
}
