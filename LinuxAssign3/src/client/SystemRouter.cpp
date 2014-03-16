#include "ServerSystemRouter.h"
#include "Packets.h"
#include "utils.h"
#include "ConnectionManager.h"
#include "ChannelSystem.h"
#define MAIN_CHANNEL 0
int num_channels = 1;
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
    pthread_t 	thread_recv;
    pthread_t 	thread_send;
    pthread_t   thread_input[MAX_CHANNELS];

    int         input_pipe[MAX_CHANNELS][2];
    int 		recv_pipe[2];
    int         send_pipe[2];
  
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    uint32_t    type;

	pipe(input_pipe[MAIN_CHANNEL]);
	pipe(recv_pipe);
    pipe(send_pipe);

    // dispatch_thread(SendSystem, (void*)cmdata, &thread_send); // send thread
    // dispatch_thread(RecvSystem, (void*)input_pipe, &thread_recv); // recv thread
    // dispatch_thread(InputManager, (void*)input_pipe, &thread_input[i]); // main channel thread


    max_fd = input_pipe[MAIN_CHANNEL][READ] > recv_pipe[READ] ? input_pipe[MAIN_CHANNEL][READ] > recv_pipe[READ];
    max_fd = max_fd > send_pipe[READ] ? max_fd : send_pipe[READ];
	
    FD_ZERO(&listen_fds);
    FD_SET(input_pipe[MAIN_CHANNEL][READ], &listen_fds);
    FD_SET(send_pipe[READ], &listen_fds);
    FD_SET(recv_pipe[READ], &listen_fds);

    type = -1;

    while(1)
    {
    	int ret;
        active = listen_fds;
    	ret = select(max_fd + 1, &active, NULL, NULL, NULL);

        for(int i = 0; i < num_channels; i++)
        {
            if(ret && FD_ISSET(input_pipe[i][READ], &active))
        	{  
                if(read_pipe(input_pipe[i][READ], &type, TYPE_SIZE) == -1){}

                else if(type == JOIN_CHANNEL)
                {
                    //send client join pkt to send
                }

                else if(type == QUIT_CHANNEL)
                {
                    //send client quit pkt to send
                    //exit channel
                }

                else if(type == CLIENT_MSG)
                {
                    //send c_msg_pkt to send thread
                }

                else if(type == EXIT)
                {
                    //cleanup and exit
                }
            }
        }

        if(ret && FD_ISSET(recv_pipe[READ], &active))
        {  
            if(read_pipe(input_pipe[READ], &type, TYPE_SIZE) == -1){}

            else if(type == SERVER_KICK_PKT)
            {
                //print message
                //close channel and connection
            }

            else if(type == SERVER_MSG_PKT)
            {
                //print message and to chat
            }

            else if(type == CHANNEL_INFO_PKT)
            {
                //if in channel, update client list
                //else open new channel
            }
        }

        if(ret && FD_ISSET(send_pipe[READ], &active))
        {  
            printf("Failure in send thread, exiting program.\n");
            exit(3);
        }


    }

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
void add_client(int cm_pipe)
{
    C_JOIN_PKT info_packet;

    read_pipe(cm_pipe, &info_packet, sizeof(C_JOIN_PKT));

    uint32_t type = CLIENT_ADD;

    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], info_packet.channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            write_pipe(channel_pipes[i][WRITE], &info_packet, sizeof(C_JOIN_PKT));
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

    write_pipe(cm_pipe[WRITE], &type, TYPE_SIZE);
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
    CMANAGERDATA * input_data = (CMANAGERDATA*) indata;

    uint32_t type;
    char *   temp = NULL;
    char     cmd[MAX_STRING];
    char *   channelname;
    char *   name;
    size_t   nbytes = MAX_STRING;

    while(getline(&temp, &nbytes, stdin))
    {
        sscanf(temp, "%s", cmd);
        
        if(strcmp(cmd, "/create") == 0)
        {
            type = CHANNEL_CREATE;
            write_pipe(input_data->write_pipe, &type, TYPE_SIZE);
            sscanf(temp, "%s %s %s", cmd, channelname);
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
}