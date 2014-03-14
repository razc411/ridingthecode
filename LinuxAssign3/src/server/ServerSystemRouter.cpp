#include <stdio>
#include "ServerSystemRouter.h"

int last_channel 		= 0;
int open_channels 		= 0;
int num_clients 		= 0;
int **client_list 		= (int**) malloc(sizeof(INT) * MAX_CHANNELS);
char **channel_name_list = (char**) malloc((sizeof(char) * MAX_CHANNEL_NAME) * MAX_CHANNELS));	
int   channel_pipes[MAX_CHANNELS][2];

uint32_t packet_types[] = {
	sizeof(S_KICK_PKT),
	sizeof(S_MSG_PKT),
	sizeof(S_CHANNEL_INFO_PKT),
	sizeof(C_MSG_PKT),
	sizeof(C_QUIT_PKT),
	sizeof(C_JOIN_PKT)
}

int main()
{
	pthread_t 	thread_channel[MAX_CHANNELS];
    pthread_t 	thread_cManager;
    pthread_t 	thread_input;

    int         input_pipe[2];
    int 		cm_pipe[2];
  
    int 		max_fd;

    sem_t		client_list_sem;
    sem_t       channel_name_sem;

	CMANAGERDATA * cmdata = (CMANAGERDATA*)malloc(sizeof(CMANAGERDATA));
	CHANNEL_DATA * chdata = (CHANNEL_DATA*)malloc(sizeof(CHANNEL_DATA));

	pipe(input_pipe);
	pipe(cm_pipe);

	if(!sem_init(&client_list_sem, 0, 1))
	{
		perror("Creating client_list_sem : ");
	}

	if(!sem_init(&channel_name_sem, 0, 1))
	{
		perror("Creating channel_name_sem : ");
	}

	setup_data(cmdata, chdata);

    dispatch_thread(ConnectionManager, (void*)cmdata, thread_cManager);
    dispatch_thread(InputManager, (void*)input_pipe, thread_input);

    max_fd = input_pipe[READ] > cm_pipe[READ] ? input_pipe[READ] : cm_pipe[READ];

	FD_ZERO(&listen_fds);
    FD_SET(input_pipe[READ], &listen_fds);
    FD_SET(cm_pipe[READ], &listen_fds);

    while(1)
    {
    	int ret;
        active = listen_fds;
    	ret = select(max_fd + 1, &active, NULL, NULL, NULL);

    	if(ret && FD_ISSET(input_pipe[READ], &active))
    	{
    		if(type == NEW_CHANNEL)
    		{
    			//read channel name
    			add_channel(chdata, &max_fd, &listen_fds);
    		}
    		// if command is kick

    		// if command is close channel

    		// if command is close server

    		ret--;
    	}

    	if(ret && FD_ISSET(cm_pipe[READ], &active))
    	{
    		// respond to connection manager data

    		// new client, add to list, notify proper channel
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

}

void* InputManager(void * pipes)
{
	//recieve keyboard input
}

void setup_data(CHANNEL_DATA * chdata, CMANAGERDATA * cmdata)
{
	cmdata->client_list = client_list;
	cmdata->channel_name_list = channel_name_list;
	cmdata->write_pipe = cm_pipe[WRITE];
	cmdata->read_pipe = cm_pipe[READ];

	chdata->client_list = client_list;
	chdata->channel_name_list = channel_name_list;
	chdata->client_list_sem = client_list_sem;
	chdata->channel_name_sem = channel_name_sem;
}

void add_channel(CHANNEL_DATA * chdata, int * max_fd, int * listen_fds)
{
	pipe(channel_pipes[open_channels]);
	chdata->channel_num = open_channels;
	chdata->write_pipe = channel_pipes[open_channels][WRITE];
	chdata->read_pipe = channel_pipes[open_channels][READ];

	if(open_channels > last_channel)
    {
    	*max_fd = *max_fd > channel_pipes[last_channel][READ] ? *max_fd > channel_pipes[last_channel][READ];
    	FD_SET(channel_pipes[last_channel][READ], &listen_fds);
	}

	last_channel == open_channels;

	dispatch_thread(ChannelManager, (void*)chdata, thread_channel[open_channels++]);
}