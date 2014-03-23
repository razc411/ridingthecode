#ifndef SERVER_DEFS_H
#define SERVER_DEFS_H

#define QUEUE 5

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <cstring>
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
void close_server(int input_pipes[2]);
void* InputManager(void * pipes);
void add_channel(int * max_fd, fd_set * listen_fds, int input_pipe);
void add_client(int cm_pipe, void * join_req, int joined);
void write_type(int pipe, int type);
void reform_router_lists();

void* ChannelManager(void * chdata);
void new_channel_client(int read_pipe);
void channel_client_kick(int sock, int client, char * msg);
void process_incoming_message(int sock, C_MSG_PKT * client_msg, int c_num);
void process_client_quit(int sock, C_QUIT_PKT * client_msg, int c_num);
void process_add_client(int cm_pipe, int * max_fd, fd_set * listen_fds);
void reform_lists();


#endif