#include "utils.h"


typedef struct{
	char **channel_name_list;
	int channel_num;
	sem_t channel_name_sem;
	sem_t client_list_sem;
	int write_pipe;
	int read_pipe;
} CHANNEL_DATA;

typedef struct{
	int write_pipe;
	int read_pipe;
} CMANAGERDATA;

void kick_client(int read_pipe);
void channel_close(int input_pipe);
void close_server(int cm_pipe[2], int input_pipe[2]);
void* InputManager(void * pipes);
void add_channel(CHANNEL_DATA * chdata, int * max_fd, fd_set * listen_fds, int input_pipe);
void add_client(int cm_pipe);
