#include "client_defs.h"
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

int num_channels = 1;
int current_channel = 1;
static pthread_t thread_input[MAX_CHANNELS];
static int input_pipe[MAX_CHANNELS][2];

int main()
{
    pthread_t 	thread_recv;
    pthread_t 	thread_send;

    int 		recv_pipe[2];
    int         send_pipe[2];
  
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    uint32_t    type;

	pipe(input_pipe[MAIN_CHANNEL]);
    
    dispatch_thread(InputManager, (void*)input_pipe, &thread_input[i]); // main channel thread

    max_fd = input_pipe[MAIN_CHANNEL][READ];
   
    FD_ZERO(&listen_fds);
    FD_SET(input_pipe[MAIN_CHANNEL][READ], &listen_fds);

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
                    join_channel(&listen_fds, &max_fd);
                }

                else if(type == QUIT_CHANNEL)
                {
                    //quit_channel();
                }

                else if(type == CLIENT_MSG)
                {
                    
                    if(tcp_send(sd, sizeof(C_MSG_PKT), (char*)info_packet) != CLIENT_MSG)
                    {
                        perror("Failed to send message");
                        return;
                    }
                }

                else if(type == EXIT)
                {
                    break;
                }
            }
        }

        if(ret && FD_ISSET(recv_pipe[READ], &active))
        {  
            if(read_pipe(input_pipe[READ], &type, TYPE_SIZE) == -1){}

            else if(type == SERVER_KICK_PKT)
            {
                //print message
                printf("You have been kicked by the server.\n");

                //close channel and connection
                channel_close(input_pipe[READ]);
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

    for(int i = 0; i < num_channels; i++)
    {
        close(input_pipe[i][READ]);
        close(input_pipe[i][WRITE]);
    }

    close(recv_pipe[READ]);
    close(recv_pipe[WRITE]);
    close(send_pipe[READ]);
    close(send_pipe[WRITE]);

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
int connect_to_server(){
    int sd = -1;
    struct hostent  *hp;
    struct sockaddr_in server;

    if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot create socket");
        exit(1);
    }

    bzero((char *)&server, sizeof(struct sockaddr_in));

    server.sin_family = AF_INET;
    server.sin_port = htons(TCP_PORT);

    if ((hp = gethostbyip("127.0.0.1")) == NULL)
    {
        fprintf(stderr, "Unknown server ip address\n");
        exit(1);
    }
    bcopy(hp->h_addr, (char *)&server.sin_addr, hp->h_length);

    // Connecting to the server
    if (connect (sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        fprintf(stderr, "Can't connect to server\n");
        perror("connect");
        exit(1);
    }

    return sd;
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
void join_channel(int * listen_fds, int * max_fd)
{
    int sd;
    int input_pipe[2];
    C_JOIN_PKT * info_packet;
    S_CHANNEL_INFO_PKT * c_info_packet;
    
    if((sd = connect_to_server()) != 0)
    {
        printf("Failed to connect, exiting program.\n");
        return;
    }
    
    if(tcp_send(sd, sizeof(C_JOIN_PKT), (char*)info_packet) != CLIENT_JOIN)
    {
        perror("Failed to join channel");
        return;
    }

    if(tcp_recieve(sd, (char*)c_info_packet) < 0)
    {
        printf("Failed to retrieve channel info packet.\n");
    }
    
    if(c_info_packet.code == CONNECTION_REJECTED)
    {
        printf("Channel join failed: Server connection rejected.");
    }

    pipe(input_pipe);
    input_pipes[current_channel][READ] = input_pipe[READ];
    input_pipes[current_channel++][WRITE] = input_pipe[WRITE];

    CHANNEL_DATA cdata;
    cdata.write_pipe = input_pipe[WRITE];
    cdata.read_pipe = input_pipe[READ];
    memcpy(cdata.channelname, c_info_packet->channel_name, sizeof(c_info_packet->chanel_name));
    memcpy(cdata.client_list, c_info_packet->channel_clients, sizeof(c_info_packet->channel_clients));

    *max_fd = *max_fd > input_pipes[current_channel][READ] ? *max_fd : input_pipes[current_channel][READ];
    FD_SET(input_pipes[current_channel][READ], listen_fds);

    dispatch_thread(InputManager, (void*)cdata, &thread_input[i]);
    num_channels++;
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
void quit_channel(int cm_pipe)
{
    C_QUIT_PKT * info_packet = (C_QUIT_PKT*)malloc(sizeof(C_QUIT_PKT));

    //send client quit pkt to send
    if(tcp_send(sd, sizeof(C_QUIT_PKT), (char*)info_packet) != CLIENT_QUIT)
    {
        perror("Failed to quit channel");
        return;
    }
    channel_close(cm_pipe);
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
        else
        {
            //send message
        }
    }

    return NULL;
}