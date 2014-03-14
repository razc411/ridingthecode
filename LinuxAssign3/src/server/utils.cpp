/*
	Wrapper function for resolve host.
*/
int resolve_host(IPaddress *ip_addr, const uint16_t port, const char *host_ip_string)
{
    if(SDLNet_ResolveHost(ip_addr, host_ip_string, port) == -1)
    {
        fprintf(stderr, "SDLNet_ResolveHost: %s\n", SDLNet_GetError());
        return -1;
    }
    return 0;
}
/**
 * Creates an IPaddress struct holding the IP address and port information for the SDL network functions.
 *
 * @param[in] num_sockets The number of sockets to be added to the set.
 * @param[in] ...         A list of size @a num_sockets of TCPsocket and UDPsocket structs 
 *                        to add to the set.
 *
 * @return An SDLNet_SocketSet pointer on success, or NULL on failure.
 *
 * @designer Shane Spoor
 * @author Shane Spoor
 *
 * @date Febuary 15, 2014
 */
SDLNet_SocketSet make_socket_set(int num_sockets, ...)
{
    int i;
	va_list socket_list; 	
	SDLNet_SocketSet set = SDLNet_AllocSocketSet(num_sockets);

	if(!set)
	{
		fprintf(stderr, "SDLNet_AllocSocketSet: %s\n", SDLNet_GetError());
		return NULL;
	}
	
	va_start(socket_list, num_sockets);
	for(i = 0; i < num_sockets; i++)
	{
		if(SDLNet_AddSocket(set, va_arg(socket_list, SDLNet_GenericSocket)) == -1)
		{
			fprintf(stderr, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
			return NULL;
		}
	}

	va_end(socket_list);
	return set;
}

/*
	Wrapper function for SDL TCP Recv.
*/
int recv_tcp(TCPsocket sock, void *buf, size_t bufsize)
{
	int numread = SDLNet_TCP_Recv(sock, buf, bufsize);
	
	if(numread == -1)
	{
    	fprintf(stderr, "SDLNet_TCP_Recv: %s\n", SDLNet_GetError());
    	return cnt_errno = ERR_TCP_RECV_FAIL;
	}
	else if(numread == 0){
        fprintf(stderr, "recv_tcp: Connection closed or reset.\n");
		return cnt_errno = ERR_CONN_CLOSED;
	}
	
	return numread;
}

/*
	Wrapper for checking a sdl socket set.
*/
int check_sockets(SDLNet_SocketSet set)
{
	int numready = SDLNet_CheckSockets(set, 100000);

	if(numready == -1)
	{
		fprintf(stderr, "SDLNet_CheckSockets: %s\n", SDLNet_GetError());
		perror("SDLNet_CheckSockets");
	}

	return numready;
}

/*
	Wrapper for recving tcp packets.
*/
void *recv_tcp_packet(TCPsocket sock, uint32_t *packet_type, uint64_t *timestamp)
{
	void     *packet;
	int      numread;
    uint32_t packet_size;

	numread = recv_tcp(sock, packet_type, sizeof(uint32_t));
	if(numread < 0){
		cnt_errno = ERR_TCP_RECV_FAIL;
		return NULL;
	}
	else if(numread == 0){
		cnt_errno = ERR_CONN_CLOSED;
		return NULL;
	}

	packet_size = packet_sizes[(*packet_type) - 1];

	if((packet = malloc(packet_size)) == NULL)
	{
		perror("recv_ tcp_packet: malloc");
		cnt_errno = errno;
		return NULL;
	}

	numread = recv_tcp(sock, packet, packet_size);
	return packet;
}

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