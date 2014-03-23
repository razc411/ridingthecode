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

static int num_channels = 0;
static int current_channel = 0;
static pthread_t thread_input[MAX_CHANNELS];
static int input_pipes[MAX_CHANNELS][2];
static int socket_list[MAX_CHANNELS];
static char clientname[MAX_USER_NAME];
char ** channel_names = (char**) malloc(sizeof(char*) * MAX_CHANNELS);
char *** channel_users = (char***) malloc(sizeof(char**) * MAX_CHANNELS);

int main(int argc, char ** argv)
{
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    if(argc != 2 || strlen(argv[1]) > 20)
    {
        printf("Please input a user name. Can be no longer than 20 characters.");
        exit(2);
    }

    memcpy(clientname, argv[1], MAX_USER_NAME);


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
        active = listen_fds;
    	select(max_fd + 1, &active, NULL, NULL, NULL);

        check_input_pipes(&active, &listen_fds, max_fd);
        check_output_sockets(&active);
        
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
void join_channel(fd_set * listen_fds, int * max_fd, int input_pipe, int c_num)
{
    int sd;
    int pipes[2];
    int type;
    C_JOIN_PKT * info_packet;
    S_CHANNEL_INFO_PKT * c_info_packet;
    
    if(!(sd = connect_to_server()))
    {
        printf("Failed to connect, exiting program.\n");
        return;
    }

    info_packet = (C_JOIN_PKT*)recieve_cjoin(input_pipe);
    memcpy(info_packet->client_name, clientname, MAX_USER_NAME);

    if(!write_packet(sd, CLIENT_JOIN_PKT, info_packet))
    {
        perror("Failed to join channel");
        return;
    }

    if((c_info_packet = (S_CHANNEL_INFO_PKT*)read_packet(sd, &type)) == NULL)
    {
        printf("Failed to retrieve channel info packet.\n");
        return;
    }
    
    if(c_info_packet->code == CONNECTION_REJECTED)
    {
        printf("Channel join failed: Server connection rejected.");
        return;
    }

    pipe(pipes);
    input_pipes[current_channel][READ] = pipes[READ];
    input_pipes[current_channel][WRITE] = pipes[WRITE];

    CHANNEL_DATA cdata;
    cdata.write_pipe = pipes[WRITE];
    cdata.read_pipe = pipes[READ];

    setup_channel_variables(c_info_packet);

    *max_fd = *max_fd > input_pipes[current_channel][READ] ? *max_fd : input_pipes[current_channel][READ];
    FD_SET(sd, listen_fds);

    socket_list[current_channel] = sd;
    dispatch_thread(InputManager, (void*)&cdata, &thread_input[current_channel++]);
    num_channels++;

    free(c_info_packet);
    free(info_packet);
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
void setup_channel_variables(S_CHANNEL_INFO_PKT * c_info)
{
   channel_names[current_channel] = (char*)malloc(sizeof(char) * MAX_CHANNEL_NAME);
   memcpy(channel_names[current_channel], c_info->channel_name, MAX_CHANNEL_NAME);
   
   channel_users[current_channel] = (char**)malloc(sizeof(char*) * MAX_CLIENTS);

   for(int i = 0; i < c_info->num_clients; i++)
   {
        channel_users[current_channel][i] = (char*)malloc(sizeof(char*) * MAX_USER_NAME);
        memcpy(channel_users[current_channel][i], c_info->channel_clients[i], MAX_USER_NAME);
   }
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
void check_input_pipes(fd_set * active, fd_set * listen_fds, int max_fd)
{
    int type = -1;

    for(int i = 0; i < num_channels; i++)
    {
        if(FD_ISSET(input_pipes[i][READ], active))
        {  
            if(read_pipe(input_pipes[i][READ], &type, TYPE_SIZE) == -1){}

            else if(type == JOIN_CHANNEL)
            {
                join_channel(listen_fds, &max_fd, input_pipes[i][READ], i);
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
                    return;
                }
            }

            else if(type == EXIT)
            {
                break;
            }
        }
    }
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
void check_output_sockets(fd_set * active)
{
    for(int i = 0; i < num_channels; i++)
    {
        if(FD_ISSET(socket_list[i], active))
        {  
            int type;
            char * packet = (char*) read_packet(socket_list[i], &type);

            if(type == SERVER_KICK_PKT)
            {
                //channel_close(input_pipe[READ]);
                close(socket_list[i]);
            }

            else if(type == SERVER_MSG_PKT)
            {
                printf("Message recieved for channel %d.\n", i);
            }

            else if(type == CHANNEL_INFO_PKT)
            {
                printf("Channel %d updated with new client listings.\n", i);
            }
            free(packet);
        }
    }
}

void render_windows()
{
    
}