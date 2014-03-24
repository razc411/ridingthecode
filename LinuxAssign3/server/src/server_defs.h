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
void channel_close(int input_pipe);
void close_server(int input_pipes[2]);
void* InputManager(void * pipes);
void add_channel(int * max_fd, fd_set * listen_fds, int input_pipe);
void add_client(int cm_pipe, void * join_req, int joined);

void* ChannelManager(void * chdata);
void process_incoming_message(int sock, C_MSG_PKT * client_msg, int c_num, int * channel_num, int * current_clients);
void process_client_quit(int sock, C_QUIT_PKT * client_msg, int c_num, int * channel_num, int * current_clients);
void process_add_client(int cm_pipe, int * max_fd, fd_set * listen_fds, int * channel_num, int * current_clients);
void display_users(int input_pipe);
void reject_client(int client_sd, C_JOIN_PKT * info_pkt);

#endif