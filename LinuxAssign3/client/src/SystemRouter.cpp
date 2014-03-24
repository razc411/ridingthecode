#include "client_defs.h"

/*------------------------------------------------------------------------------------------------------------------
--      SOURCE FILE:    SystemRouter.cpp -An application that will allow users to join chat channel
--                                      with asynchronous socket communcation using Select
--
--      PROGRAM:        client_chat
--
--      FUNCTIONS:
--                      int main(int argc, char ** argv)
--                      int connect_to_server()
--                      void join_channel(fd_set * listen_fds, int * max_fd, int input_pipe)
--                      void setup_channel_variables(S_CHANNEL_INFO_PKT * c_info)
--                      void check_input_pipes(fd_set * active, fd_set * listen_fds, int * max_fd)
--                      void check_output_sockets(fd_set * active, fd_set * listen_fds)
--                      void quit_channel(fd_set * listen_fds, int code)
--                      void display_incoming_message(S_MSG_PKT * packet)
--
--      DATE:           March 7, 2014
--
--      REVISIONS:      (Date and Description)
--
--      DESIGNER:       Tim Kim
--
--      PROGRAMMER:     Tim Kim
--
--      NOTES:
--      The user can join chat channel by typing /join [channelname] and can start chatting with other
--      client who are in the same channel. They can also leave channel and join another one.
----------------------------------------------------------------------------------------------------------------------*/

static int in_channel = false;
static int input_pipe[2];
static int client_socket;
static char clientname[MAX_USER_NAME];
static char channel_name[MAX_CHANNEL_NAME];
static bool logging = false;
char ** channel_users = (char**) malloc(sizeof(char*) * MAX_CLIENTS);

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       main
--
--      DATE:           March 8 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      int main(int argc, char ** argv)
--
--      RETURNS:        int
--
--      NOTES:
--      Takes an ip address and user name as arguments. Opens read and write pipe in separate threads.
----------------------------------------------------------------------------------------------------------------------*/
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
        check_output_sockets(&active, &listen_fds);
        
    }

    return 0;
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       connect_to_server
--
--      DATE:           March 7 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      int connect_to_server()
--
--      RETURNS:        int
--
--      NOTES:
--      Creates a socket, connects to the remote server by using an ip address and port number
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
--      FUNCTION:       join_channel
--
--      DATE:           March 9 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void join_channel(fd_set * listen_fds, int * max_fd, int input_pipe)
--
--      RETURNS:        void
--
--      NOTES:
--      Sends join channel control packet to the server, and does error handling when joining channel
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
        perror("Failed to join channel.\n");
        return;
    }

    if((c_info_packet = (S_CHANNEL_INFO_PKT*)read_packet(sd, &type)) == NULL)
    {
        printf("Failed to retrieve channel info packet.\n");
        return;
    }
    
    if(c_info_packet->code == CONNECTION_REJECTED)
    {
        printf("Channel join failed: Server connection rejected.\n");
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
--      FUNCTION:       setup_channel_variables
--
--      DATE:           March 9 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void setup_channel_variables(S_CHANNEL_INFO_PKT * c_info)
--
--      RETURNS:        void
--
--      NOTES:
--      Initializes each chat channel information
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
--      FUNCTION:       check_input_pipes
--
--      DATE:           March 11 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void check_input_pipes(fd_set * active, fd_set * listen_fds, int * max_fd)
--
--      RETURNS:        void
--
--      NOTES:
--      Reads control packet from the input pipe and handle each messages accordingly.
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
            quit_channel(listen_fds, CLIENT_QUIT);
        }

        else if(type == CLIENT_MSG)
        {
            if(in_channel == true)
            {
                C_MSG_PKT * c_msg;
                c_msg = (C_MSG_PKT*)recieve_cmsg(input_pipe[READ]);
                if(write_packet(client_socket, CLIENT_MSG_PKT, c_msg) != CLIENT_MSG)
                {
                    perror("Failed to send message");
                    return;
                }
            }
            else
            {
                printf("You must be in a channel to send messages.\n");
            }
        }

        else if(type == EXIT)
        {
            quit_channel(listen_fds, EXIT);
            exit(1);
        }
    }  
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       check_input_pipes
--
--      DATE:           March 11 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void check_input_pipes(fd_set * active, fd_set * listen_fds, int * max_fd)
--
--      RETURNS:        void
--
--      NOTES:
----------------------------------------------------------------------------------------------------------------------*/
void check_output_sockets(fd_set * active, fd_set * listen_fds)
{
   
    if(FD_ISSET(client_socket, active))
    {  
        int type;
        void * packet = (char*) read_packet(client_socket, &type);

        if(type == SERVER_MSG_PKT)
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

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       quit_channel
--
--      DATE:           March 11 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void quit_channel(fd_set * listen_fds, int code)
--
--      RETURNS:        void
--
--      NOTES:
--      It will print quit channel message and write quit control packet to the server
----------------------------------------------------------------------------------------------------------------------*/
void quit_channel(fd_set * listen_fds, int code)
{
    printf("You have left the channel %s.\n", channel_name);
    
    C_QUIT_PKT pkt;
    pkt.code = code;
    write_packet(client_socket, CLIENT_QUIT_PKT, &pkt);

    in_channel = false;
    memset(channel_name, 0, MAX_CHANNEL_NAME);
    FD_CLR(client_socket, listen_fds);
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       display_incoming_message
--
--      DATE:           March 11 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void display_incoming_message(S_MSG_PKT * packet)
--
--      RETURNS:        void
--
--      NOTES:
--      displays the chat incoming message
----------------------------------------------------------------------------------------------------------------------*/
void display_incoming_message(S_MSG_PKT * packet)
{
    printf("%s : %s", packet->client_name, packet->msg);
    if(logging == true)
    {

    }
}