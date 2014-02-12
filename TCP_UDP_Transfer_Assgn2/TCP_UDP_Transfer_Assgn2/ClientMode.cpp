/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: TCP_UDP_Transfer_Assgn2
--
--	PROGRAM : A TCP/UDP File Transfer Server/Client
--
--	FUNCTIONS :
--		void init_client(HWND hwnd)
--		int client_connect(HWND hwnd)
--		void init_transfer(HWND hwnd)
--		void tcp_deliver_packets(WSABUF * wsaBuffers, SOCKET sock, int totalBytesRead, int packet_size, int buffer_count, int mode)
--		void udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, WSABUF * buffers)
--		int receive_acknowledge(HWND hwnd)
--	
--	DATE: Febuary 6, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	Deals with the program when it is started as a client in the client/server connection. Intializes, connects and sends packets
--	out from the client to the server. If using UDP, will take into consideration both the setting for Server Port and Client port.
--	Only considers the Client port on the TCP side.
--
--	The UDP here has minor reliability, in that dropped packets will be resent based on acknowledgements from the server side, along with
--	a header that accompanies both TCP and UDP. 
--
--	Data can only be sent when the Client side is intialized. Problems may arise if the user disconnects and jumps between UDP/TCP.
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
static time_t startTime; // transfer start time
static time_t endTime;	//	transfer end time
static double seconds;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_client
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void init_client(HWND hwnd) , takes the parent HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      Intializes the client, the type of intialization depends on the chosen protocol in the settings. Binds whenever
--		the connection is TCP.
----------------------------------------------------------------------------------------------------------------------*/
void init_client(HWND hwnd){
	DWORD Ret;
	SOCKADDR_IN InternetAddr;
	WSADATA wsaData;
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0){
		activity("WSAStartup failed on client.\n", EB_STATUSBOX);
		return;
	}

	switch(st->protocol){
	case TCP:
		st->client_socket = socket(PF_INET, SOCK_STREAM, 0);
		break;
	case UDP:
		st->server_socket = socket(AF_INET, SOCK_DGRAM, 0);
		if ((st->client_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET){
			activity("Failed to create transfer socket.\n", EB_STATUSBOX);
		}
		
		InternetAddr.sin_family = AF_INET;
		InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		InternetAddr.sin_port = htons(atoi(st->server_port));

		if (bind(st->server_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR){
			activity("bind() failed on receiving socket.\n", EB_STATUSBOX);
		}
		break;
	}

	SetClassLongPtr(hwnd, 0, (LONG)st);

	if(st->client_socket == INVALID_SOCKET){
		activity("Failed to create receiving socket.\n", EB_STATUSBOX);
	}

	if (st->protocol == UDP){
		activity("UDP Client intialized, ready to transfer.\n", EB_STATUSBOX);
		return;
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORT);

	if (bind(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR){
		activity("bind() failed on client.\n", EB_STATUSBOX);
	}

	activity("TCP Client intialized, ready to connect.\n", EB_STATUSBOX);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: client_connect
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void client_connect(HWND hwnd), takes the parent window HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      When the client is in TCP mode, will connect the client to the TCP server. Returns messages on failure. After 
--		performing a connect the client is ready to transfer files.
----------------------------------------------------------------------------------------------------------------------*/
void client_connect(HWND hwnd){
	SOCKADDR_IN InternetAddr;
	hostent *hp;
	int iRc;
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	memset((char *)&InternetAddr, 0, sizeof(SOCKADDR_IN));

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_port = htons(atoi(st->client_port));
	
	if ((hp = gethostbyname(st->client_send_ip)) == NULL)
	{
		activity("Could not find the specified server address.\n", EB_STATUSBOX);
		return;
	}

	memcpy((char *)&InternetAddr.sin_addr, hp->h_addr, hp->h_length);

	iRc = connect(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
	if (iRc != 0){
		activity("Failed to connect to server.\n", EB_STATUSBOX);
		return;
	}

	setsockopt(st->client_socket, SOL_SOCKET, SO_REUSEADDR, 0, 0);
	SetClassLongPtr(hwnd, 0, (LONG)st);
	activity("Connected to server.\n", EB_STATUSBOX);
	
	return;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_transfer
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void init_transfer(HWND hwnd), takes the parent window HWND.
--
--      RETURNS: void
--
--      NOTES:
--      Intializes a UDP or TCP transfer based on the current settings. Will generate dummy packets if the program is in
--		dummy packet transfer mode, otherwise reads a file and generates packets for the file. Passes the created packets
--		to their respective UDP/TCP functions.
----------------------------------------------------------------------------------------------------------------------*/
void init_transfer(HWND hwnd){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	int status;
	char msg[MAX_SIZE];
	HANDLE hf;
	DWORD numBytesRead = 0;
	int  totalBytesRead = 0;
	DWORD packetsizes[] = {1024, 4096, 20000, 60000};
	const int packet_size = packetsizes[st->packet_size];
	
	if (st->mode == FILEMODE){
		grab_file(hwnd, &hf);
	}

	activity("Sending data...\n", EB_STATUSBOX);
	WSABUF * wsaBuffers = (LPWSABUF)malloc(sizeof(WSABUF)* MAX_PACKETS);
	
	char * buff = (char*)malloc(sizeof(char)* packet_size);

	int i = 0, buffer_count = 0;
	while (1){
		if (st->mode == FILEMODE){
			status = ReadFile(hf, buff, packet_size, &numBytesRead, NULL);
			if (numBytesRead == 0){
				CloseHandle(hf);
				break;
			}
			wsaBuffers[i].buf = buff;
			wsaBuffers[i++].len = numBytesRead;
			totalBytesRead += numBytesRead;
			buffer_count++;
		}
		else{
			buffer_count = atoi(st->times_to_send);
			
			for (int g = 0; g < packet_size; g++){
				buff[g] = 'p';
			}
			buff[packet_size - 1] = '\0';

			for (int p = 0; p < buffer_count; p++){
				wsaBuffers[p].buf = buff;
				wsaBuffers[p].len = packet_size;
			}
			totalBytesRead = packet_size * buffer_count;
			break;
		}
	}
	
	sprintf_s(msg, "Sending %d bytes in %d packets.\n", totalBytesRead, buffer_count);
	activity(msg, EB_STATUSBOX);

	if(st->protocol == UDP)
		udp_deliver_packets(hwnd, totalBytesRead, packet_size, buffer_count, wsaBuffers);
	else
		tcp_deliver_packets(hwnd, wsaBuffers, st->client_socket, totalBytesRead, packet_size, buffer_count, st->mode);

	free(wsaBuffers);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: tcp_deliver_packets
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void tcp_deliver_packets(HWND hwnd, WSABUF * wsaBuffers, SOCKET sock, int totalBytesRead, int packet_size, int buffer_count, int mode)
--
--      RETURNS: void
--
--      NOTES:
--		Takes the parent window HWND, the total bytes to be transferred (totalBytesRead), the current packet size, the amount of packets
--		an an array of WSABUFs each containing a packet of data.
--
--		Delivers TCP packets to the specified host. Intially sends a header containing the int variables passed to the function
--		before sending the actual data. Must be passed an array of WSABUFs, with each WSABUF containing a packet of data.
----------------------------------------------------------------------------------------------------------------------*/
void tcp_deliver_packets(HWND hwnd, WSABUF * wsaBuffers, SOCKET sock, int totalBytesRead, int packet_size, int buffer_count, int mode){
	
	int status;
	char msg[MAX_SIZE];
	DWORD  numBytesSent = 0;

	char flags[HEADER_SIZE];
	memset(flags, '\0', HEADER_SIZE);
	sprintf_s(flags, ";%d,%d,%d,%d;", totalBytesRead, packet_size, buffer_count, mode);
	
	LPWSABUF wsaHeaderBuf = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaHeaderBuf->len = HEADER_SIZE;
	wsaHeaderBuf->buf = flags;

	if ((status = WSASend(sock, wsaHeaderBuf, 1, &numBytesSent, NULL, NULL, NULL)) < 0){
		sprintf_s(msg, "Error %d in TCP WSASend(header) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
	}

	time(&startTime);

	for (int p = 0; p < buffer_count; p++){
		if ((status = WSASend(sock, &wsaBuffers[p], 1, &numBytesSent, NULL, NULL, NULL)) < 0){
			sprintf_s(msg, "Error %d in TCP WSASend(buffer) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return;
		}
	}

	time(&endTime);
	seconds = difftime(endTime, startTime);
	activity("Data transmission complete.\n", EB_STATUSBOX);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: udp_deliver_packets
--
--      DATE: Febuary 6 2014
--      REVISIONS: Countless. Most recent, turning it into somewhat reliable UDP.
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, WSABUF * buffers)
--
--      RETURNS: void
--
--      NOTES:
--      Takes the parent window HWND, the total bytes to be transferred (totalBytesRead), the current packet size, the amount of packets
--		an an array of WSABUFs each containing a packet of data.
--		
--		Delivers the packets over UDP to the specified IP and port, and has some reliability for dropped packets by 
--		incorporating an acknowledgement system as well as a header packet containing the info passed to the function. 
--		If an error occurs in the recvfrom portion of the acknowledgement, consider the transfer dead and restart the program.
--
--		* WILL loop endlessly if the user decides to terminate the server during a transfer. Will continously wait for ACKs.
----------------------------------------------------------------------------------------------------------------------*/
void udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, WSABUF * buffers){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	
	int packets_lost = 0;
	int status;
	char msg[MAX_SIZE];
	DWORD  numBytesSent = 0;
	struct sockaddr_in sin;

	char flags[HEADER_SIZE];
	memset(flags, '\0', HEADER_SIZE);
	sprintf_s(flags, ";%d,%d,%d,%d;", totalBytesRead, packet_size, buffer_count, st->mode);

	LPWSABUF wsaHeaderBuf = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaHeaderBuf->len = HEADER_SIZE;
	wsaHeaderBuf->buf = flags;

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(st->client_port));

	if ((sin.sin_addr.s_addr = inet_addr(st->client_send_ip)) == INADDR_NONE)
	{
		activity("Failed to find address\n", EB_STATUSBOX);
		return;
	}
	
	while (1){ // transferring the header packet
		if ((status = WSASendTo(st->client_socket, wsaHeaderBuf, 1, &numBytesSent, 0, (struct sockaddr *)&sin, sizeof(sin), NULL, NULL)) < 0){
			sprintf_s(msg, "Error %d in TCP WSASend(header) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
		}

		if (receive_acknowledge(hwnd) == 1) { break; } 
		packets_lost++;
	}
	
	time(&startTime);

	for (int p = 0; p < buffer_count; p++){
		while (1){ // transferring the actual data
			if ((status = WSASendTo(st->client_socket, &buffers[p], 1, &numBytesSent, 0, (struct sockaddr *)&sin, sizeof(sin), NULL, NULL)) < 0){
				sprintf_s(msg, "Error %d in sendto(buffer) with return of %d\n", WSAGetLastError(), status);
				activity(msg, EB_STATUSBOX);
				return;
			}

			if (receive_acknowledge(hwnd) == 1){ break; }
			packets_lost++;
		}
	}

	time(&endTime);
	seconds = difftime(endTime, startTime);
	sprintf_s(msg, "Data transmission completed in %f seconds.", seconds);
	activity(msg, EB_STATUSBOX);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: receive_acknowledge
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int receive_acknowledge(HWND hwnd), where hwnd is the HWND of the main parent window.
--
--      RETURNS: int, returns 1 if an acknowledgement is received. 
--
--      NOTES:
--      Checks if the packet received after sending a packet is an "ACK" packet. An ACK packet contains the text ";ACK;".
--		*Yes this may be prone to problems, for our purposes it makes it easier. Can easily be changed.
--		*Only used on the UDP side, provides reliability for the client. TCP already does this for us.
----------------------------------------------------------------------------------------------------------------------*/
int receive_acknowledge(HWND hwnd){
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	char ack[6];
	int status, ackrecv;
	
	if ((status = recvfrom(st->server_socket, ack, ACK_SIZE, 0, 0, 0)) == -1){
		activity("Error in acknowledgement, run before it blows up!", EB_STATUSBOX);
	}

	ackrecv = (strcmp(ack, ";ACK;") == 0) ? 1 : 0;
	return ackrecv;
}