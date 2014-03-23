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

static int in_channel = false;
static int input_pipe[2];
static int client_socket;
static char clientname[MAX_USER_NAME];
static char channel_name[MAX_CHANNEL_NAME];
char ** channel_users = (char**) malloc(sizeof(char*) * MAX_CLIENTS);

int main(int argc, char ** argv)
{
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;
    pthread_t   thread_input;

    if(argc != 2 || strlen(argv[1]) > 20)
    {
        printf("Please input a user name. Can be no longer than 20 characters.");
        exit(2);
    }

    memcpy(clientname, argv[1], MAX_USER_NAME);

	pipe(input_pipe);
    THREAD_DATA * idata = (THREAD_DATA*)malloc(sizeof(THREAD_DATA));
    idata->write_pipe = input_pipe[WRITE];
    idata->read_pipe = input_pipe[READ];

    dispatch_thread(InputManager, (void*)idata, &thread_input); // main channel thread

    max_fd = input_pipe[READ];
   
    FD_ZERO(&listen_fds);
    FD_SET(input_pipe[READ], &listen_fds);

    while(1)
    {
        active = listen_fds;
    	select(max_fd + 1, &active, NULL, NULL, NULL);

        check_input_pipes(&active, &listen_fds, &max_fd);
        check_output_sockets(&active);
        
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
void join_channel(fd_set * listen_fds, int * max_fd, int input_pipe)
{
    int sd;
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

    setup_channel_variables(c_info_packet);

    *max_fd = *max_fd > sd ? *max_fd : sd;
    client_socket = sd;
    FD_SET(client_socket, listen_fds);

    in_channel = true;

    printf("Channel %s joined.\n", c_info_packet->channel_name);
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
   memcpy(channel_name, c_info->channel_name, MAX_CHANNEL_NAME);

   for(int i = 0; i < c_info->num_clients; i++){
        channel_users[i] = (char*)malloc(sizeof(char) * MAX_USER_NAME);
        memcpy(channel_users[i], c_info->channel_clients[i], MAX_USER_NAME);
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
void check_input_pipes(fd_set * active, fd_set * listen_fds, int * max_fd)
{
    int type = -1;

    if(FD_ISSET(input_pipe[READ], active))
    {  
        if(read_pipe(input_pipe[READ], &type, TYPE_SIZE) == -1){}

        else if(type == JOIN_CHANNEL)
        {
            if(in_channel == true)
            {
               printf("Must leave the current channel before joining another.\n");
               return;
            }
            join_channel(listen_fds, max_fd, input_pipe[READ]);
        }

        else if(type == QUIT_CHANNEL)
        {
            //quit_channel();
        }

        else if(type == CLIENT_MSG)
        {
            C_MSG_PKT * c_msg;
            c_msg = (C_MSG_PKT*)recieve_cmsg(input_pipe[READ]);
            if(write_packet(client_socket, CLIENT_MSG_PKT, c_msg) != CLIENT_MSG)
            {
                //perror("Failed to send message");
                return;
            }
        }

        else if(type == EXIT)
        {
            return;
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
   
    if(FD_ISSET(client_socket, active))
    {  
        int type;
        void * packet = (char*) read_packet(client_socket, &type);

        if(type == SERVER_KICK_PKT)
        {
            //channel_close(input_pipe[READ]);
            close(client_socket);
        }

        else if(type == SERVER_MSG_PKT)
        {
            display_incoming_message((S_MSG_PKT*)packet);
        }

        else if(type == CHANNEL_INFO_PKT)
        {
            printf("Channel %s updated with new client listings.\n", channel_name);
        }
        free(packet);
    }
}

void render_windows()
{
    
}

void display_incoming_message(S_MSG_PKT * packet)
{
    printf("%s : %s\n", packet->client_name, packet->msg);
    //retrieve timestamp and add that
}