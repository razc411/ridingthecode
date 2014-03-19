#include "server_defs.h"
#include "utils.h"

int packet_sizes[] = {
	sizeof(S_KICK_PKT),
	sizeof(S_MSG_PKT),
	sizeof(C_MSG_PKT),
	sizeof(C_QUIT_PKT),
	sizeof(C_JOIN_PKT)
};
/*
	Wrapper for starting threads.
*/
int dispatch_thread(void *(*function)(void *), void *params, pthread_t *handle)
{
	if(pthread_create(handle, NULL, function, params) == -1)
	{
		perror("dispatch_thread");
		return -1;
	}

	pthread_detach(*handle);
	return 0;
}

int read_pipe(int fd, void *buf, size_t count)
{
    int ret = read(fd, buf, count);
    
    if(ret == -1)
        perror("read_pipe");        

    return ret;
}

int write_pipe(int fd, const void *buf, size_t count)
{
    int ret = write(fd, buf, count);
    
    if(ret == -1)
        perror("write_pipe");        

    return ret;
}
/*
	Creates an accept socket.
*/
int create_accept_socket(){

    int listen_sd, arg = 1;
    struct sockaddr_in server;

    if ((listen_sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("Cannot Create Socket!");
    	return -1;
    }
    if (setsockopt (listen_sd, SOL_SOCKET, SO_REUSEADDR, &arg, sizeof(arg)) == -1)
    {
        perror("setsockopt");
        return -2;
    }

    bzero((char *)&server, sizeof(struct sockaddr_in));
    server.sin_family = AF_INET;
    server.sin_port = htons(TCP_PORT);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(listen_sd, (struct sockaddr *)&server, sizeof(server)) == -1)
    {
        perror("bind error");
        return -3;
    }

    listen(listen_sd, 5);

    return listen_sd;
}
/*
	Accepts a new client
*/
int accept_new_client(int listen_sd)
{
	unsigned int client_len;
	int client_sd = 0;
	struct sockaddr_in client_addr;

	client_len = sizeof(client_addr);
	if ((client_sd = accept(listen_sd, (struct sockaddr *) &client_addr, &client_len)) == -1)
	{
	    perror("accept error");
	    return -1;
	}
	
	printf(" Remote Address:  %s\n", inet_ntoa(client_addr.sin_addr));

	return client_sd;
}

int tcp_recieve(int sockfd, char * packet)
{
	int type_size = TYPE_SIZE;
	int bytes_to_read;
	int * type = NULL;
	int numread = 0;
	
	while ((numread = read(sockfd, &type, type_size)) > 0)
	{
		type += numread;
		type_size -= numread;
	}

	packet = (char*) malloc(sizeof(packet_sizes[*type]));
	bytes_to_read = packet_sizes[*type];

	while ((numread = read(sockfd, packet, packet_sizes[*type])) > 0)
	{
		packet += numread;
		bytes_to_read -= numread;
	}

	if(bytes_to_read > 0)
	{
		fprintf(stderr, "Failed to read packet.\n");
		return -2;
	}

	return *type;
}

int write_packet(int sockfd, int type, void * packet)
{
	write(sockfd, &type, TYPE_SIZE);
	write(sockfd, packet, packet_sizes[type]);

	return type;
}
