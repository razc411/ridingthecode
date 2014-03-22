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

int num_channels = 0;
int current_channel = 0;
static pthread_t thread_input[MAX_CHANNELS];
static int input_pipes[MAX_CHANNELS][2];
static int socket_list[MAX_CHANNELS];
static char clientname[MAX_USER_NAME] = "doof";

int main()
{
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    int    type;

	pipe(input_pipes[MAIN_CHANNEL]);
    THREAD_DATA * idata = (THREAD_DATA*)malloc(sizeof(THREAD_DATA));
    idata->write_pipe = input_pipes[MAIN_CHANNEL][WRITE];
    idata->read_pipe = input_pipes[MAIN_CHANNEL][READ];

    dispatch_thread(InputManager, (void*)idata, &thread_input[num_channels++]); // main channel thread
    current_channel++;
    
    max_fd = input_pipes[MAIN_CHANNEL][READ];
   
    FD_ZERO(&listen_fds);
    FD_SET(input_pipes[MAIN_CHANNEL][READ], &listen_fds);

    while(1)
    {
        type = -1;
        active = listen_fds;
    	select(max_fd + 1, &active, NULL, NULL, NULL);

        for(int i = 0; i < num_channels; i++)
        {
            if(FD_ISSET(input_pipes[i][READ], &active))
        	{  
                if(read_pipe(input_pipes[i][READ], &type, TYPE_SIZE) == -1){}

                else if(type == JOIN_CHANNEL)
                {
                    join_channel(&listen_fds, &max_fd, input_pipes[i][READ]);
                }

                else if(type == QUIT_CHANNEL)
                {
                    //quit_channel();
                }

                else if(type == CLIENT_MSG)
                {
                    C_MSG_PKT c_msg;
                    read_pipe(input_pipes[i][READ], &c_msg, sizeof(C_MSG_PKT));
                    if(write_packet(socket_list[i], CLIENT_MSG_PKT, (char*)&c_msg) != CLIENT_MSG)
                    {
                        perror("Failed to send message");
                        return -1;
                    }
                }

                else if(type == EXIT)
                {
                    break;
                }
            }
        }

        for(int i = 0; i < num_channels; i++)
        {
            if(FD_ISSET(socket_list[i], &active))
            {  
                char * packet;
                type = tcp_recieve(socket_list[i], packet);

                if(type == SERVER_KICK_PKT)
                {
                    //channel_close(input_pipe[READ]);
                    close(socket_list[i]);
                }

                else if(type == SERVER_MSG_PKT)
                {
                    write_packet(input_pipes[i][WRITE], type, packet);
                    printf("Message passed to input channel %d.\n", i);
                }

                else if(type == CHANNEL_INFO_PKT)
                {
                    write_packet(input_pipes[i][WRITE], type, packet);
                    printf("Channel update passed to input channel %d.\n", i);
                }
            }
        }
    }

    for(int i = 0; i < num_channels; i++)
    {
        close(input_pipes[i][READ]);
        close(input_pipes[i][WRITE]);
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

    if ((hp = gethostbyname("127.0.0.1")) == NULL)
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
void join_channel(fd_set * listen_fds, int * max_fd)
{
    int sd;
    int input_pipe[2];
    C_JOIN_PKT * info_packet = (C_JOIN_PKT*) malloc(sizeof(C_JOIN_PKT));
    S_CHANNEL_INFO_PKT * c_info_packet;
    
    if(!(sd = connect_to_server()))
    {
        printf("Failed to connect, exiting program.\n");
        return;
    }
    
    info_packet->client_name = "dogguy";
    info_packet->

    if(!write_packet(sd, CLIENT_JOIN_PKT, (char*)info_packet))
    {
        perror("Failed to join channel");
        return;
    }

    if(tcp_recieve(sd, (char*)c_info_packet) < 0)
    {
        printf("Failed to retrieve channel info packet.\n");
        return;
    }
    
    if(c_info_packet->code == CONNECTION_REJECTED)
    {
        printf("Channel join failed: Server connection rejected.");
        return;
    }

    pipe(input_pipe);
    input_pipes[current_channel][READ] = input_pipe[READ];
    input_pipes[current_channel][WRITE] = input_pipe[WRITE];

    CHANNEL_DATA cdata;
    cdata.write_pipe = input_pipe[WRITE];
    cdata.read_pipe = input_pipe[READ];
    memcpy(cdata.channelname, c_info_packet->channel_name, sizeof(c_info_packet->channel_name));
    memcpy(cdata.client_list, c_info_packet->channel_clients, sizeof(c_info_packet->channel_clients));

    *max_fd = *max_fd > input_pipes[current_channel][READ] ? *max_fd : input_pipes[current_channel][READ];
    FD_SET(sd, listen_fds);

    socket_list[current_channel] = sd;
    dispatch_thread(InputManager, (void*)&cdata, &thread_input[current_channel++]);
    num_channels++;
}