#ifndef CLIENT_DEFS_H
#define CLIENT_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <string>
#include <netdb.h>
#include <pthread.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <SDL2/SDL.h>
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"
#include "packets.h"
#include "utils.h"

#define SCREEN_WIDTH 	1280
#define SCREEN_HEIGHT 	768
#define SCREEN_BPP 		32
#define MAIN_CHANNEL 	0

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
int connect_to_server();
void join_channel(fd_set * listen_fds, int * max_fd);

#endif
