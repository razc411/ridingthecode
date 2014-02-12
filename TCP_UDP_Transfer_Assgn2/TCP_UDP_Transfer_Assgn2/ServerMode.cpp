/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: ServerMode.cpp
--
--	PROGRAM : TCP/UDP Transfer Win32 Assign2
--
--	FUNCTIONS :
--		void init_server(HWND hwnd)
--		int socket_event(HWND hwnd, WPARAM wParam, LPARAM lParam)
--		int read_tcp(HWND hwnd, SOCKET sock)
--		int read_udp(HWND hwnd, SOCKET sock)
--		int grab_header(HWND hwnd, SOCKET sock)
--		int init_tcp_receive(HWND hwnd)
--		void accept_data(HWND hwnd, WPARAM wParam)
--		int transfer_completion(HWND hwnd, int mode)
--		int process_header(HWND hwnd, SOCKET recv)
--		void grab_header_info(char * hdBuffer)
--		void acknowledge(HWND hwnd)
--
--	DATE: Febuary 6, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	Server side of the UDP/TCP transfer program. Processes WM_SOCKET messages and responds accordingly based on the 
--	intial header transfered on both the UDP and TCP protocols. Processes TCP data and saves/tosses based on the header mode
--	and on the UDP side will do the same, except each UDP packet is acknowledged by the server to the client. 
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
LPSOCKET_INFORMATION SocketInfo; // struct which holds crucial header data
static time_t startTime; // transfer start time
static time_t endTime;	//	transfer end time
static double seconds;
char * buffer; // buffer allocated for holding transfer data
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_server
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void init_server(HWND hwnd) , takes the parent HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      Intializes the server, the type of intialization depends on the chosen protocol in the settings. Listens to the socket 
--		whenever the connection is TCP.
----------------------------------------------------------------------------------------------------------------------*/
void init_server(HWND hwnd){

	DWORD Ret;
	SOCKET Listen, Send;
	WSADATA wsaData;
	SOCKADDR_IN InternetAddr;

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0){
		activity("WSAStartup failed on server.\n", EB_STATUSBOX);
		return;
	}

	switch (st->protocol){
	case TCP:
		Listen = socket(PF_INET, SOCK_STREAM, 0);
		WSAAsyncSelect(Listen, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);
		break;
	case UDP:
		Send = socket(AF_INET, SOCK_DGRAM, 0);
		Listen = socket(AF_INET, SOCK_DGRAM, 0);
		WSAAsyncSelect(Listen, hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
		break;
	}
	if (Listen == INVALID_SOCKET){
		activity("Failed to create client socket.\n", EB_STATUSBOX);
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(atoi(st->server_port));

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		activity("bind() failed on server.\n", EB_STATUSBOX);
		return;
	}

	if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL){
		activity("GlobalAlloc() failed with error\n", EB_STATUSBOX);
		return;
	}

	SocketInfo->header_received = 0;

	if (st->protocol == TCP){
		if (listen(Listen, 5))
		{
			activity("listen() failed on server.\n", EB_STATUSBOX);
			return;
		}
	}
	else{
		st->server_socket = Listen;
		st->client_socket = Send;
		SetClassLongPtr(hwnd, 0, (LONG)st);
		activity("UDP Server intiated", EB_STATUSBOX);
	}
	activity("TCP Server intiated", EB_STATUSBOX);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: socket_event
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void socket_event(HWND hwnd, WPARAM wParam, LPARAM lParam), where wParam is the socket and lParam is the error message.
--		The hwnd is the parent HWND.
--
--      RETURNS: void
--
--      NOTES:
--      Asynchronously responds to socket accept and read events on the server side of the program. Will reallocate the 
--		SocketInfo structure whenever a transfer ends. 
----------------------------------------------------------------------------------------------------------------------*/
void socket_event(HWND hwnd, WPARAM wParam, LPARAM lParam){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	char msg[MAX_SIZE];

	if (WSAGETSELECTERROR(lParam))
	{
		sprintf_s(msg, "WSAGETSELECTERROR %d, connection failed.\n", WSAGETSELECTERROR(lParam));
		activity(msg, EB_STATUSBOX);
		closesocket(st->server_socket);
	}
	else
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:
			accept_data(hwnd, wParam);
			break;
		case FD_READ:
			if (st->protocol == TCP){
				if (read_tcp(hwnd, st->server_socket) >= 1){
					if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL){
						activity("GlobalAlloc() failed with error\n", EB_STATUSBOX);
					}
					SocketInfo->header_received = 0; // transmission ends, program waits for a new header
				}
			}
			else{
				if (read_udp(hwnd, st->server_socket) >= 1){
					if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL){
						activity("GlobalAlloc() failed with error\n", EB_STATUSBOX);
					}
					SocketInfo->header_received = 0;
				}
			}
		}
	}
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read_tcp
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int read_tcp(HWND hwnd, WPARAM wParam, SOCKET sock)
--
--      RETURNS: int, many return values, if the value is >= to 1 - it is assumed the transfer has ended. Otherwise it continues.
--
--      NOTES:
--		Interface for reading the TCP data and header. 
----------------------------------------------------------------------------------------------------------------------*/
int read_tcp(HWND hwnd, SOCKET sock){
	if (SocketInfo->header_received == 0){
		return grab_header(hwnd, sock);
	}
	return init_tcp_receive(hwnd);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read_udp
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int read_udp(HWND hwnd, WPARAM wParam, SOCKET sock)
--
--      RETURNS: int, many return values, if the value is >= to 1 - it is assumed the transfer has ended. Otherwise it continues.
--
--      NOTES:
--      Interface for reading the UDP data and header. 
----------------------------------------------------------------------------------------------------------------------*/
int read_udp(HWND hwnd, SOCKET sock){
	if (SocketInfo->header_received == 0){
		return grab_header(hwnd, sock);
	}
	return init_udp_receive(hwnd);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: grab_header
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int grab_header(HWND hwnd, SOCKET sock)
--
--      RETURNS: int, returns -1 if the header was not successfully grabbed. Returns 0 if it was.
--
--      NOTES:
--		Grabs the intial header and sets up the program to recieve the actual data.
----------------------------------------------------------------------------------------------------------------------*/
int grab_header(HWND hwnd, SOCKET sock){
	if (process_header(hwnd, sock) == -1){
		return -1;
	}
	buffer = (char*)malloc(sizeof(char)* (SocketInfo->packet_size * SocketInfo->packets));
	memset(buffer, 0, (SocketInfo->packet_size * SocketInfo->packets));
	time(&startTime);
	SocketInfo->header_received = 1;
	SocketInfo->BytesRECV = 0;
	SocketInfo->totalRecv = 0;
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: accept_data
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void accept_data(HWND hwnd, WPARAM wParam)
--
--      RETURNS: void
--
--      NOTES:
--      Accepts a new connection over TCP on the listening socket. Called whenever a FD_ACCEPT message is grabbed from WM_SOCKET.
--		The wParam in this instance is the socket that sent out the FD_ACCEPT request. The HWND belongs to the parent window.
----------------------------------------------------------------------------------------------------------------------*/
void accept_data(HWND hwnd, WPARAM wParam){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	if ((st->server_socket = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
	{
		activity("Failed to accept() connection.\n", EB_STATUSBOX);
		return;
	}

	setsockopt(st->server_socket, SOL_SOCKET, SO_REUSEADDR, 0, 0);
	WSAAsyncSelect(st->server_socket, hwnd, WM_SOCKET, FD_READ | FD_CLOSE);
	SetClassLongPtr(hwnd, 0, (LONG)st);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_tcp_recieve
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int init_tcp_receive(HWND hwnd, WPARAM wParam) takes the parent HWND as an argument.
--
--      RETURNS: int, returns values that are dealt with by read_tcp. Positive values deem a transfer as over.
--
--      NOTES:
--      Takes a HWND to the parent window.
--	
--		Sets up the system to accept a TCP data transfer, reads the packet and adds it to the main buffer. Once the transfer
--		has been completed a message is printed or the data is saved, based on the specified mode.
----------------------------------------------------------------------------------------------------------------------*/
int init_tcp_receive(HWND hwnd){

	DWORD Flags = 0;
	int status;
	char msg[MAX_SIZE];
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	char * tempBuffer = (char*)malloc(sizeof(char)* SocketInfo->packet_size);
	SocketInfo->DataBuf.len = SocketInfo->packet_size;
	SocketInfo->DataBuf.buf = tempBuffer;

	if ((status = WSARecv(st->server_socket, &SocketInfo->DataBuf, 1, &SocketInfo->BytesRECV, &Flags, NULL, NULL)) == SOCKET_ERROR){
			sprintf_s(msg, "Error %d in TCP WSARecv(data) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return 0;
	}

	if (SocketInfo->mode == FILEMODE){
		SocketInfo->DataBuf.buf[SocketInfo->BytesRECV - 1] = '\0';
		strcat_s(buffer + SocketInfo->totalRecv, SocketInfo->BytesRECV, SocketInfo->DataBuf.buf);
	}

	SocketInfo->packets -= 1;
	SocketInfo->totalRecv += SocketInfo->BytesRECV;

	if (SocketInfo->totalRecv == SocketInfo->total_size){
		time(&endTime);
		seconds = difftime(endTime, startTime);
		return transfer_completion(hwnd, SocketInfo->mode);
	}
	return -2; // packets remaining
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_udp_recieve
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int init_udp_receive(HWND hwnd)
--
--      RETURNS: int, returns values that are dealt with by read_udp. Positive values deem a transfer as over.
--
--      NOTES:
--      Takes a HWND to the parent window.
--
--		Sets up the system to accept a UDP data transfer, reads the packet, acknowledges it and adds it to the main buffer. Once the transfer
--		has been completed a message is printed or the data is saved, based on the specified mode.
----------------------------------------------------------------------------------------------------------------------*/
int init_udp_receive(HWND hwnd){
	DWORD Flags = 0;
	int status;
	char msg[MAX_SIZE];
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	char ack[6] = ";ACK;";

	char * tempBuffer = (char*)malloc(sizeof(char)* SocketInfo->packet_size);
	SocketInfo->DataBuf.len = SocketInfo->packet_size;
	SocketInfo->DataBuf.buf = tempBuffer;

	if ((status = WSARecvFrom(st->server_socket, &SocketInfo->DataBuf, 1, &SocketInfo->BytesRECV, &Flags, NULL, NULL, NULL, NULL)) == SOCKET_ERROR){
		sprintf_s(msg, "Error %d in TCP WSARecv(data) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
		return 0;
	}
	acknowledge(hwnd);

	if (SocketInfo->mode == 0){
		SocketInfo->DataBuf.buf[SocketInfo->BytesRECV - 1] = '\0';
		strcat_s(buffer + SocketInfo->totalRecv, SocketInfo->BytesRECV, SocketInfo->DataBuf.buf);
	}

	SocketInfo->packets -= 1;
	SocketInfo->totalRecv += SocketInfo->BytesRECV;

	if (SocketInfo->totalRecv == SocketInfo->total_size || SocketInfo->packets == 0){
		time(&endTime);
		return transfer_completion(hwnd, SocketInfo->mode);
	}
	return -2; // packets remaining
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: transfer_completion
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int transfer_completion(HWND hwnd, int mode), takes the parent HWND as an argument and the transfer mode.
--
--      RETURNS: int, returns a positive value over 0, causing the program to deem the transfer as completed.
--
--      NOTES:
--		Called on transfer completion, gets the time the transfer took and saves the data if in FILEMODE, and does nothing
--		with the data if in packet mode.
--
--		Prints info on the transfer and updates the statistics.
----------------------------------------------------------------------------------------------------------------------*/
int transfer_completion(HWND hwnd, int mode){

	char msg[MAX_SIZE];
	
	if (mode == 0){
		save_file(hwnd, buffer, SocketInfo->totalRecv);
		sprintf_s(msg, "Recieved %d bytes. File transfer completed in %f seconds.\n", SocketInfo->totalRecv, seconds);
		activity(msg, EB_STATUSBOX);
		return 3;
	}

	sprintf_s(msg, "Recieved %d bytes. Garbage transfer completed in %f seconds.\n", SocketInfo->totalRecv, seconds);
	activity(msg, EB_STATUSBOX);
	return 2; // transfer completed, closing connection
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: process_header
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int process_header(HWND hwnd, SOCKET recv), takes the parent HWND as an argument and the socket the header was sent to.
--
--      RETURNS: void
--
--      NOTES:
--      Retrieves the header datagram from the passed in socket. If using TCP, just grabs it. If on UDP, the header is grabbed then
--		acknowledged. If no header is successfully grabbed, the function is called on every WM_SOCKET message until it is.
----------------------------------------------------------------------------------------------------------------------*/
int process_header(HWND hwnd, SOCKET recv){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	DWORD RecvBytes = 0, Flags = 0;
	int status, server_len;
	struct	sockaddr_in server;

	char msg[MAX_SIZE];
	char * hdBuffer = (char*)malloc(sizeof(char)* HEADER_SIZE);
	memset(hdBuffer, 0, HEADER_SIZE);

	LPWSABUF wsaBuffer = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaBuffer->len = HEADER_SIZE;
	wsaBuffer->buf = hdBuffer;

	if (st->protocol == TCP){
		if ((status = WSARecv(recv, wsaBuffer, 1, &RecvBytes, &Flags, NULL, NULL)) == SOCKET_ERROR){
			sprintf_s(msg, "Error %d in TCP WSARecv(header) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
		}

		if (wsaBuffer->buf[0] != ';'){ // I really should have used an uncommon character here
			return -1;
		}
	}
	else{
		server_len = sizeof(server);
		if ((status = WSARecvFrom(st->server_socket, wsaBuffer, 1, &RecvBytes, &Flags, (struct sockaddr *)&server, &server_len, NULL, NULL)) == SOCKET_ERROR){
			return -1;
		}
		acknowledge(hwnd);
	}

	grab_header_info(hdBuffer);
	return 1;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: grab_header_info
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void grab_header_info(char * hdBuffer), takes a buffer with the retrieved transfer header.
--
--      RETURNS: void
--
--      NOTES:
--		Retrieves the header data from the header and sets the SocketInfo globally malloced struct. Grabs total bytes sent,
--		packet size, total packets and the times to send.
----------------------------------------------------------------------------------------------------------------------*/
void grab_header_info(char * hdBuffer){

	char psize[100], tBytes[100], mode[100], pcks[100];

	int i = 1, p = 0, j = 0, q = 0, r = 0;

	//grabs total bytes
	for (; hdBuffer[i] != ','; i++){
		tBytes[j++] = hdBuffer[i];
	}
	tBytes[j] = '\0';

	//grabs packet size
	for (i += 1; hdBuffer[i] != ','; i++){
		psize[p++] = hdBuffer[i];
	}
	psize[p] = '\0';

	//grabs total packets
	for (i += 1; hdBuffer[i] != ','; i++){
		pcks[r++] = hdBuffer[i];
	}
	pcks[r] = '\0';

	//grabs times the packets are sent
	for (i += 1; hdBuffer[i] != ';'; i++){
		mode[q++] = hdBuffer[i];
	}
	mode[q] = '\0';

	SocketInfo->initPackets = atoi(pcks);
	SocketInfo->total_size = atoi(tBytes);
	SocketInfo->packet_size = atoi(psize);
	SocketInfo->mode = atoi(mode);
	SocketInfo->packets = atoi(pcks);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: acknowledge
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void acknowledge(HWND hwnd), takes the parent HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      Sends an acknowledgement packet out on the specified client_port/ip. Only called on UDP transfers.
----------------------------------------------------------------------------------------------------------------------*/
void acknowledge(HWND hwnd){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	struct sockaddr_in sin;
	int status;
	char ack[ACK_SIZE] = ";ACK;";

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(st->client_port));

	if ((sin.sin_addr.s_addr = inet_addr(st->client_send_ip)) == INADDR_NONE)
	{
		activity("Failed to find transfer address\n", EB_STATUSBOX);
		return;
	}

	if ((status = sendto(st->client_socket, ack, ACK_SIZE, 0, (const sockaddr *)&sin, sizeof(sin))) < 0){
		activity("Failed to acknowledge, run before it blows up!\n", EB_STATUSBOX);
	}
}