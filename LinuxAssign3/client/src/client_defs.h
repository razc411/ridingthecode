#ifndef CLIENT_DEFS_H
#define CLIENT_DEFS_H
// Contains definitions specific to the client.
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
#define MAXIP			20

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

void* InputManager(void * indata);

void check_input_pipes(fd_set * active, fd_set * listen_fds, int * max_fd);
void check_output_sockets(fd_set * active, fd_set * listen_fds);

int connect_to_server();
void display_incoming_message(S_MSG_PKT * packet);
void join_channel(fd_set * listen_fds, int * max_fd, int input_pipe, int c_id);
void quit_channel(fd_set * listen_fds, int code);
void setup_channel_variables(S_CHANNEL_INFO_PKT * c_info);

#endif
