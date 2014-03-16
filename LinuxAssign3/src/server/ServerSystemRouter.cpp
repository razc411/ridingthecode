#include "server_defs.h"
#include "utils.h"

int last_channel 		= 0;
int open_channels 		= 0;
int num_clients 		= 0;
char **channel_name_list = (char**) malloc((sizeof(char) * MAX_CHANNEL_NAME) * MAX_CHANNELS);	
int * router_socket_list = (int*) malloc(sizeof(int) * MAX_CLIENTS);
static int channel_pipes[MAX_CHANNELS][2];
static pthread_t   thread_channel[MAX_CHANNELS];

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
int main()
{
    pthread_t 	thread_input;

    int         input_pipe[2];
  
    int 		max_fd, client_sd;
    fd_set      listen_fds;
    fd_set      active;

    uint32_t    type = -1;

    THREAD_DATA * idata = (THREAD_DATA*)malloc(sizeof(THREAD_DATA));
    CHANNEL_DATA * chdata = (CHANNEL_DATA*)malloc(sizeof(CHANNEL_DATA*));

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
        active = listen_fds;
    	select(max_fd + 1, &active, NULL, NULL, NULL);

        if(FD_ISSET(listen_sd, &active))
        {
            client_sd = accept_new_client(listen_sd);
            add_client(client_sd);
        }

    	if(FD_ISSET(input_pipe[READ], &active))
    	{  

            if(read_pipe(input_pipe[READ], &type, TYPE_SIZE) == -1){}

            else if(type == CHANNEL_CREATE)
    		{
    			add_channel(chdata, &max_fd, &listen_fds, input_pipe[READ]);
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
                break;
            }
    	}

    	for(int i = 0; i < open_channels; i++)
    	{
    		if(FD_ISSET(channel_pipes[i][READ], &active))
    		{
    			//respond to channel quit
    			//clean channel data up
    		}
    	}
    }

    free(idata);
    close(input_pipe[READ]);
    close(input_pipe[WRITE]);

    return 0;
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
void add_client(int client_sd)
{
    C_JOIN_PKT * info_packet = (C_JOIN_PKT*)malloc(sizeof(C_JOIN_PKT));

    if(tcp_recieve(client_sd, sizeof(C_JOIN_PKT), (char*)info_packet) != CLIENT_ADD)
    {
        perror("Failed to add client at tcp_recieve");
        return;
    }

    uint32_t type = CLIENT_ADD;

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
void add_channel(CHANNEL_DATA * chdata, int * max_fd, fd_set * listen_fds, int input_pipe)
{
    char temp[MAX_CHANNEL_NAME];

    read_pipe(input_pipe, &temp, MAX_CHANNEL_NAME);
    memcpy(channel_name_list[open_channels], temp, MAX_CHANNEL_NAME);
    chdata->channel_num = open_channels;

	pipe(channel_pipes[open_channels]);
	chdata->write_pipe = channel_pipes[open_channels][WRITE];
	chdata->read_pipe = channel_pipes[open_channels][READ];

	if(open_channels > last_channel)
    {
    	*max_fd = *max_fd > channel_pipes[last_channel][READ] ? *max_fd : channel_pipes[last_channel][READ];
    	FD_SET(channel_pipes[last_channel][READ], listen_fds);
	}

	last_channel = open_channels;

	dispatch_thread(ChannelManager, (void*)chdata, &thread_channel[open_channels++]);

    printf("Channel Added. Thread now open.\n");
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
void kick_client(int input_pipe)
{
    char clientname[MAX_USER_NAME];
    char channel_name[MAX_CHANNEL_NAME];
    char msg[MAX_STRING];
    uint32_t type = CLIENT_KICK;

    read_pipe(input_pipe, &clientname, MAX_USER_NAME);
    read_pipe(input_pipe, &channel_name, MAX_CHANNEL_NAME);
    read_pipe(input_pipe, &msg, MAX_STRING);
    
    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);    
            write_pipe(channel_pipes[i][WRITE], clientname, MAX_USER_NAME);
            write_pipe(channel_pipes[i][WRITE], msg, MAX_STRING);
            break;
        }
    }
        
    --num_clients;

    printf("Client kick msg passed to channel.\n");
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
void channel_close(int input_pipe)
{
    int channel_num;
    uint32_t type = CHANNEL_CLOSE;

    read_pipe(input_pipe, &channel_num, sizeof(int));

    write_pipe(channel_pipes[channel_num][WRITE], &type, TYPE_SIZE);

    printf("Channel %s closed.\n", channel_name_list[channel_num]);
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
void close_server(int cm_pipe[2], int input_pipe[2])
{
    uint32_t type = CHANNEL_CLOSE;
    for(int i = 0; i < open_channels; i++)
    {
        write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
    }

    type = SERVER_EXIT;

    write_pipe(input_pipe[WRITE], &type, TYPE_SIZE);

    for(int i = 0; i < open_channels; i++)
    {
        close(channel_pipes[i][READ]);
        close(channel_pipes[i][WRITE]);
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
    char *   channelname = NULL;
    char *   name = NULL;
    size_t   nbytes = MAX_STRING;

    while(getline(&temp, &nbytes, stdin))
    {
        sscanf(temp, "%s", cmd);
        
        if(strcmp(cmd, "/create") == 0)
        {
            type = CHANNEL_CREATE;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
        else if(strcmp(cmd, "/kick") == 0)
        {
            type = CLIENT_KICK;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s %s", cmd, channelname, name);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
            write_pipe(input_data->write_pipe, name, MAX_USER_NAME);
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
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s", cmd, channelname);
            write_pipe(input_data->write_pipe, channelname, MAX_CHANNEL_NAME);
        }
    }

    return NULL;
}