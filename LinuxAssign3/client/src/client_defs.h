#ifndef CLIENT_DEFS_H
#define CLIENT_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <string>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "packets.h"
#include "utils.h"

#define SCREEN_WIDTH 	1280
#define SCREEN_HEIGHT 	768
#define SCREEN_BPP 		32
#define MAIN_CHANNEL 	0
#define LOG				13

typedef struct{
	int write_pipe;
	int read_pipe;
} THREAD_DATA;

typedef struct{
	int write_pipe;
	int read_pipe;
	char * channelname;
	char * client_list;
} CHANNEL_DATA;

void check_output_sockets(fd_set * active, fd_set * listen_fds);
void setup_channel_variables(S_CHANNEL_INFO_PKT * c_info);
void* InputManager(void * indata);
int connect_to_server();
void join_channel(fd_set * listen_fds, int * max_fd, int input_pipe, int c_id);
void check_input_pipes(fd_set * active, fd_set * listen_fds, int * max_fd);
int connected_join_request(int client, int input_pipe);
void display_incoming_message(S_MSG_PKT * packet);
void quit_channel(fd_set * listen_fds, int code);
#endif
