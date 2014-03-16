#include "ChannelSystem.h"

void* ChannelManager(void * chdata)
{
	int 		max_fd;
    fd_set      listen_fds;
    fd_set      active;

    max_fd = input_pipe[READ] > cm_pipe[READ] ? input_pipe[READ] : cm_pipe[READ];

	FD_ZERO(&listen_fds);
    FD_SET(input_pipe[READ], &listen_fds);
    FD_SET(cm_pipe[READ], &listen_fds);

    type = -1;

    while(1)
    {
    	int ret;
        active = listen_fds;
    	ret = select(max_fd + 1, &active, NULL, NULL, NULL);

    	if(ret && FD_ISSET(_pipe[READ], &active))
    	{

    	}  
    }


}