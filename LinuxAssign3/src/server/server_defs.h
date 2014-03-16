#ifndef SERVER_DEFS_H
#define SERVER_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>
#include <SDL2/SDL_net.h>
#include <SDL2/SDL.h>
#include <string>
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
} CMANAGERDATA;

//System router headers
void kick_client(int read_pipe);
void channel_close(int input_pipe);
void close_server(int cm_pipe[2], int input_pipe[2]);
void* InputManager(void * pipes);
void add_channel(CHANNEL_DATA * chdata, int * max_fd, fd_set * listen_fds, int input_pipe);
void add_client(int cm_pipe);
void write_type(int pipe, int type);

void* ChannelManager(void * chdata);
void* ConnectionManager(void * cmdata);

// utils functions
int write_pipe(int fd, const void *buf, size_t count);
int read_pipe(int fd, void *buf, size_t count);
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle);
void *recv_tcp_packet(TCPsocket sock, uint32_t *packet_type);
int check_sockets(SDLNet_SocketSet set);
int recv_tcp(TCPsocket sock, void *buf, size_t bufsize);
SDLNet_SocketSet make_socket_set(int num_sockets, ...);
int resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string);

#endif