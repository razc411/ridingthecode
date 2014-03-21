#ifndef CLIENT_DEFS_H
#define CLIENT_DEFS_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <string.h>
//#include <semaphore.h>
#include <pthread.h>
//#include <SDL2/SDL_net.h>
//#include <SDL2/SDL.h>
//#include "SDL2/SDL.h"
//#include "SDL2/SDL_image.h"
//#include "SDL2/SDL_ttf.h"
#include <string>
#include "../../server/src/packets.h"
#include "../../server/src/utils.h"

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

int connect_to_server();
void join_channel();
void quit_channel(int cm_pipe);
void add_client(int cm_pipe);
void* InputManager(void * indata);

#endif
