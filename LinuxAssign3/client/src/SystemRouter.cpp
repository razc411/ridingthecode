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
--      Central hub of this linux chat client. Recieves input from the user by means of another thread
--      and acts accordingly, dealing with any network traffic which must occur in response, or is recieved.
--      
--      Allows the user to join, leave and send messages to channels. Also allows the user to enable logging
--      with the /log switch. All data logged will be written to chatlog.txt. Refer InputController.cpp for more
--      information on the various commands.
----------------------------------------------------------------------------------------------------------------------*/

static int in_channel = false;
static int input_pipe[2];
static int client_socket;
static char ip[MAXIP];
static char clientname[MAX_USER_NAME];
static char channel_name[MAX_CHANNEL_NAME];
static bool logging = false;
static std::ofstream myfile;
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
--      Takes an ip address and user name as arguments. Dispatches the input thread and begins
--      watching for input from the input thread or any network activity.
----------------------------------------------------------------------------------------------------------------------*/
int main(int argc, char ** argv)
{
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;
    pthread_t   thread_input;

    if(argc != 3 || strlen(argv[1]) > MAX_USER_NAME || strlen(argv[2] > MAXIP))
    {
        printf("Please input a user name and ip. User can be no longer than 20 characters.");
        exit(2);
    }

    myfile.open ("chatlog.txt");  

    memcpy(clientname, argv[1], MAX_USER_NAME);
    memcpy(ip, argv[2], 20);

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
--      Creates a socket, connects to the remote server by using an ip address and port number.
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

    if ((hp = gethostbyname(ip)) == NULL)
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
--                      fd_set * listen_fds - a pointer to the listening set for the select statement
--                      int * max_fd - the maximum value of the file descriptor to check
--                      int input_pipe - pipe to the input controller
--
--      RETURNS:        void
--
--      NOTES:
--      Retrieves a client join packet from the input thread and sends it to the server after establishing a connection.
--      A handshake occurs and the server will return whether the user can actually join the channel they requested.
--      If they can join the channel, a CONNECTION_ACCEPTED is returned, else a CONNECTION_REJECTED is returned.
--
--      Also adds the new socket to the select statement in main to watch for network events.      
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
--                          c_info - contains the variables for setting up the channel info.
--
--      RETURNS:        void
--
--      NOTES:
--      Initializes channel information.
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
--                          *active - the active descriptors
--                          *listen_fds - the listening set of descriptors
--                          *max_fd - the maximum descriptor to be checked for in select
--      RETURNS:        void
--
--      NOTES:
--      Reads control packets from the input pipe and reacts accordingly.
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

        else if(type == LOG)
        {
            (logging) ? logging = false : logging = true;
            (logging) ?  myfile.open("chatlog.txt") : myfile.close();
          
            printf("Chat logging enabled.\n");
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
                if(logging == true)
                {
                    myfile << c_msg->msg;
                }
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
            myfile.close();
            exit(1);
        }
    }  
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION:       check_output_sockets
--
--      DATE:           March 11 2014
--      REVISIONS:      none
--
--      DESIGNER:       Tim Kim
--      PROGRAMMER:     Tim Kim
--
--      INTERFACE:      void check_output_sockets(fd_set * active, fd_set * listen_fds)
--                          *active - the active descriptors
--                          *listen_fds - the listening set of descriptors
--                    
--      RETURNS:        void
--
--      NOTES:
--      Checks the connected tcp socket for any incoming data. Reacts accordingly.          
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
--                          *listen_fds - set of the listening descriptors
--                          code - the quit code to return to the server, can be CLIENT_QUIT or EXIT
--
--      RETURNS:        void
--
--      NOTES:
--      Will leave a channel and notify the server of this action. Closes the currently open socket
--      and removes it from the listening descriptors.
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
--                          *packet - struct that contains the incoming server message data
--      RETURNS:        void
--
--      NOTES:
--      Displays incoming chat messages. If logging is enabled, writes the incoming messages to file.
----------------------------------------------------------------------------------------------------------------------*/
void display_incoming_message(S_MSG_PKT * packet)
{
    char temp[MAX_STRING + MAX_USER_NAME + 3];
    sprintf(temp, "%s : %s", packet->client_name, packet->msg);
    
    printf("%s", temp);
    
    if(logging == true)
    {
        myfile << temp;
    }
}