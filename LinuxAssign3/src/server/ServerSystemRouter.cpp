#include "ServerSystemRouter.h"
#include "Packets.h"
#include "utils.h"
#include "ConnectionManager.h"
#include "ChannelSystem.h"

int last_channel 		= 0;
int open_channels 		= 0;
int num_clients 		= 0;
char **channel_name_list = (char**) malloc((sizeof(char) * MAX_CHANNEL_NAME) * MAX_CHANNELS);	
TCPsocket * socket_list = (TCPsocket*) malloc(sizeof(TCPsocket) * MAX_CLIENTS);
static int channel_pipes[MAX_CHANNELS][2];
static pthread_t   thread_channel[MAX_CHANNELS];
static uint32_t packet_types[] = {
	sizeof(S_KICK_PKT),
	sizeof(S_MSG_PKT),
	sizeof(S_CHANNEL_INFO_PKT),
	sizeof(C_MSG_PKT),
	sizeof(C_QUIT_PKT),
	sizeof(C_JOIN_PKT)
};

int main()
{
    pthread_t 	thread_cManager;
    pthread_t 	thread_input;

    int         input_pipe[2];
    int 		cm_pipe[2];
  
    int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    sem_t       channel_name_sem;

	CMANAGERDATA * cmdata = (CMANAGERDATA*)malloc(sizeof(CMANAGERDATA));
	CHANNEL_DATA * chdata = (CHANNEL_DATA*)malloc(sizeof(CHANNEL_DATA));

	pipe(input_pipe);
	pipe(cm_pipe);

	if(!sem_init(&channel_name_sem, 0, 1))
	{
		perror("Creating channel_name_sem : ");
	}

	cmdata->write_pipe = cm_pipe[WRITE];
    cmdata->read_pipe = cm_pipe[READ];

    chdata->channel_name_list = channel_name_list;
    chdata->channel_name_sem = channel_name_sem;

    dispatch_thread(ConnectionManager, (void*)cmdata, &thread_cManager);
    dispatch_thread(InputManager, (void*)input_pipe, &thread_input);

    max_fd = input_pipe[READ] > cm_pipe[READ] ? input_pipe[READ] : cm_pipe[READ];

	FD_ZERO(&listen_fds);
    FD_SET(input_pipe[READ], &listen_fds);
    FD_SET(cm_pipe[READ], &listen_fds);

    uint32_t type = -1;

    while(1)
    {
    	int ret;
        active = listen_fds;
    	ret = select(max_fd + 1, &active, NULL, NULL, NULL);

    	if(ret && FD_ISSET(input_pipe[READ], &active))
    	{  

            if(read_pipe(input_pipe[READ], &type, TYPE_SIZE) == -1){

            }

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
                close_server(cm_pipe, input_pipe);
                break;
            }

    		ret--;
    	}

    	if(ret && FD_ISSET(cm_pipe[READ], &active))
    	{
    		if(read_pipe(cm_pipe[READ], &type, TYPE_SIZE) == -1){}

    		else if(type == CLIENT_ADD)
            {
                add_client(cm_pipe[READ]);
            }

    		ret--;
    	}

    	for(int i = 0; i < open_channels; i++)
    	{
    		if(ret && FD_ISSET(channel_pipes[i][READ], &active))
    		{
    			//respond to channel quit
    			//clean channel data up
    			ret--;
    		}
    	}
    }

    free(cmdata);
    free(chdata);
    free(channel_name_list);
    free(socket_list);
    sem_close(&channel_name_sem);
    close(input_pipe[READ]);
    close(cm_pipe[READ]);
    close(input_pipe[WRITE]);
    close(cm_pipe[WRITE]);
}

void add_client(int cm_pipe)
{
    char channel_name[MAX_CHANNEL_NAME];
    TCPsocket client_socket;
    char client_name[MAX_USER_NAME];

    read_pipe(cm_pipe, channel_name, MAX_CHANNEL_NAME);
    read_pipe(cm_pipe, client_name, MAX_USER_NAME);
    read_pipe(cm_pipe, client_socket, sizeof(TCPsocket));

    uint32_t type = CLIENT_ADD;

    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], channel_name) == 0)
        {
            write_pipe(channel_pipes[i][WRITE], &type, TYPE_SIZE);
            write_pipe(channel_pipes[i][WRITE], client_name, MAX_USER_NAME);
            write_pipe(channel_pipes[i][WRITE], client_socket, sizeof(TCPsocket));
            break;
        }
    }
    
    num_clients++;
    printf("Client passed to channel for adding.\n");
}

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

void kick_client(int input_pipe)
{
    char clientname[MAX_USER_NAME];
    char channel_name[MAX_CHANNEL_NAME];
    char msg[MAX_STRING];
    int channel_num;
    uint32_t type = CLIENT_KICK;

    read_pipe(input_pipe, &clientname, MAX_USER_NAME);
    read_pipe(input_pipe, &channel_name, MAX_CHANNEL_NAME);
    read_pipe(input_pipe, &msg, MAX_STRING);
    
    for(int i = 0; i < open_channels; i++)
    {
        if(strcmp(channel_name_list[i], channel_name) == 0)
        {
            write_pipe(channel_pipes[channel_num][WRITE], &type, TYPE_SIZE);    
            write_pipe(channel_pipes[channel_num][WRITE], &clientname, MAX_USER_NAME);
            write_pipe(channel_pipes[channel_num][WRITE], &msg, MAX_STRING);
            break;
        }
    }
        
    --num_clients;

    printf("Client kick msg passed to channel.\n");
}

void channel_close(int input_pipe)
{
    int channel_num;
    uint32_t type = CHANNEL_CLOSE;

    read_pipe(input_pipe, &channel_num, sizeof(int));

    write_pipe(channel_pipes[channel_num][WRITE], &type, TYPE_SIZE);

    printf("Channel %s closed.\n", channel_name_list[channel_num]);
}

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

void* InputManager(void * pipes)
{
    //recieve keyboard input
}