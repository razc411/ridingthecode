#ifndef SERVER_DEFS_H
#define SERVER_DEFS_H

#define QUEUE 5

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
//#include <SDL2/SDL.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "packets.h"

typedef struct{
	int channel_num;
	char * channel_name;
	int write_pipe;
	int read_pipe;
} CHANNEL_DATA;

typedef struct{
	int write_pipe;
	int read_pipe;
} THREAD_DATA;

//System router headers
void kick_client(int read_pipe);
void channel_close(int input_pipe);
void close_server(int cm_pipe[2], int input_pipe[2]);
void* InputManager(void * pipes);
void add_channel(CHANNEL_DATA * chdata, int * max_fd, fd_set * listen_fds, int input_pipe);
void add_client(int cm_pipe);
void write_type(int pipe, int type);

void* ChannelManager(void * chdata);
void new_channel_client(int read_pipe);
#endif