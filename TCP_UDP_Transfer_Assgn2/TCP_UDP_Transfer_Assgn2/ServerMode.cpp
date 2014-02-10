/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: Assignment1Term4SPII.cpp
--
--	PROGRAM : Raw Terminal Input through Forks and Pipes
--
--	FUNCTIONS :
--		int main()
--		void process_input(int rawp[2], int transp[2], pid_t * children)
--		void translate_input(int frmtp[2], int transp[2])
--		void print_output(int rawp[2], int frmtp[2])
--		void fatal_error(char * error)
--
--	DATE: January 20, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	A program which disables terminal text processing and handles it instead.Several methods of character input have
--	changed, as well as methods of terminating the program.Prints out one line of raw input, then upon typing "E",
--	prints out a formatted line of text.All 'a' characters are changed to 'z', all 'z' characters are changed to 'a',
--	and control letters such as X, E, K and T are not printed in the formatted text.
--
--
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
LPSOCKET_INFORMATION SocketInfo;
time_t startTime;
char * buffer;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
void init_server(HWND hwnd){

	DWORD Ret;
	SOCKET Listen;
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
		SetClassLongPtr(hwnd, 0, (LONG)st);
		activity("UDP Server intiated", EB_STATUSBOX);
	}
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
int socket_event(HWND hwnd, WPARAM wParam, LPARAM lParam){

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
				if (read_tcp(hwnd, wParam, st->server_socket) >= 1){
					if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL){
						activity("GlobalAlloc() failed with error\n", EB_STATUSBOX);
						return 1;
					}
					SocketInfo->header_received = 0;
				}
			}
			else{
				read_udp(hwnd, wParam, st->server_socket);
			}
		}
	}

	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
int read_tcp(HWND hwnd, WPARAM wParam, SOCKET sock){
	if (SocketInfo->header_received == 0){
		time(&startTime);
		process_header(hwnd, sock);
		buffer = (char*)malloc(sizeof(char)* (SocketInfo->packet_size * SocketInfo->packets));
		memset(buffer, 0, SocketInfo->packet_size * SocketInfo->packets);
		SocketInfo->header_received = 1;
		SocketInfo->BytesRECV = 0;
		SocketInfo->totalRecv = 0;
		return 0;
	}
	return init_tcp_receive(hwnd, wParam);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
void read_udp(HWND hwnd, WPARAM wParam, SOCKET sock){
	if (SocketInfo->header_received == 0){
		time(&startTime);
		process_header(hwnd, sock);
		buffer = (char*)malloc(sizeof(char)* (SocketInfo->packet_size * SocketInfo->packets));
		memset(buffer, 0, SocketInfo->packet_size * SocketInfo->packets);
		SocketInfo->header_received = 1;
		SocketInfo->BytesRECV = 0;
		SocketInfo->totalRecv = 0;
		memset(buffer, 0, SocketInfo->total_size);
		return;
	}
	init_udp_receive(hwnd);
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
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
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
int init_tcp_receive(HWND hwnd, WPARAM wParam){

	DWORD Flags = 0;
	int status;
	char msg[MAX_SIZE];
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	char * tempBuffer = (char*)malloc(sizeof(char)* SocketInfo->packet_size);
	SocketInfo->DataBuf.len = SocketInfo->packet_size;
	SocketInfo->DataBuf.buf = tempBuffer;

	if ((status = WSARecv(st->server_socket, &SocketInfo->DataBuf, 1, &SocketInfo->BytesRECV, &Flags, NULL, NULL)) == SOCKET_ERROR){
		if (WSAGetLastError() != WSAEWOULDBLOCK){
			sprintf_s(msg, "Error %d in TCP WSARecv(data) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return -1;
		}
		return -2;
	}

	if (SocketInfo->mode == 0){
		SocketInfo->DataBuf.buf[SocketInfo->BytesRECV - 1] = '\0';
		strcat_s(buffer + SocketInfo->totalRecv, SocketInfo->BytesRECV, SocketInfo->DataBuf.buf);
	}

	SocketInfo->packets -= 1;
	SocketInfo->totalRecv += SocketInfo->BytesRECV;

	if (SocketInfo->totalRecv == SocketInfo->total_size){
		return tcp_transfer_completion(hwnd, SocketInfo->mode);
	}
	return 0; // packets remaining
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
int tcp_transfer_completion(HWND hwnd, int mode){

	time_t endTime;
	double seconds;
	char msg[MAX_SIZE];

	time(&endTime);

	seconds = difftime(endTime, startTime);

	if (mode == 0){
		save_file(hwnd, buffer, SocketInfo->total_size);
		sprintf_s(msg, "Recieved %d bytes. File transfer completed in %f seconds.\n", SocketInfo->totalRecv, seconds);
		activity(msg, EB_STATUSBOX);
		return 1;
	}

	sprintf_s(msg, "Recieved %d bytes. Garbage packet transfer completed in %f seconds.\n", SocketInfo->totalRecv, seconds);
	activity(msg, EB_STATUSBOX);
	return 2; // transfer completed, closing connection
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
int init_udp_receive(HWND hwnd){
	DWORD Flags = 0;
	int status;
	char msg[MAX_SIZE];
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	char * tempBuffer = (char*)malloc(sizeof(char)* SocketInfo->packet_size);
	SocketInfo->DataBuf.len = SocketInfo->packet_size;
	SocketInfo->DataBuf.buf = tempBuffer;

	if ((status = WSARecvFrom(st->server_socket, &SocketInfo->DataBuf, 1, &SocketInfo->BytesRECV, &Flags, NULL, NULL, NULL, NULL)) == SOCKET_ERROR){
		if (WSAGetLastError() != WSAEWOULDBLOCK){
			sprintf_s(msg, "Error %d in TCP WSARecv(data) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return -1;
		}
		return -2;
	}

	if (SocketInfo->mode == 0){
		SocketInfo->DataBuf.buf[SocketInfo->BytesRECV - 1] = '\0';
		strcat_s(buffer + SocketInfo->totalRecv, SocketInfo->BytesRECV, SocketInfo->DataBuf.buf);
	}

	SocketInfo->packets -= 1;
	SocketInfo->totalRecv += SocketInfo->BytesRECV;

	if (SocketInfo->totalRecv == SocketInfo->total_size){
		return tcp_transfer_completion(hwnd, SocketInfo->mode);
	}
	return 0; // packets remaining
}

/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
void process_header(HWND hwnd, SOCKET recv){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	DWORD RecvBytes = 0, Flags = 0;
	int recvBytes, status, server_len;
	struct	sockaddr_in server;

	char msg[MAX_SIZE];
	char * hdBuffer = (char*)malloc(sizeof(char)* HEADER_SIZE);
	memset(hdBuffer, 0, HEADER_SIZE);

	LPWSABUF wsaBuffer = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaBuffer->len = HEADER_SIZE;
	wsaBuffer->buf = hdBuffer;

	if (st->protocol == TCP){
		if ((status = WSARecv(recv, wsaBuffer, 1, &RecvBytes, &Flags, NULL, NULL)) == SOCKET_ERROR){
			if (WSAGetLastError() != WSAEWOULDBLOCK){
				sprintf_s(msg, "Error %d in TCP WSARecv(header) with return of %d\n", WSAGetLastError(), status);
				activity(msg, EB_STATUSBOX);
				return;
			}
		}
	}
	else{
		server_len = sizeof(server);
		while (1){
			if ((status = WSARecvFrom(st->server_socket, wsaBuffer, 1, &RecvBytes, &Flags, (struct sockaddr *)&server, &server_len, NULL, NULL)) == 0){
				break;
			}
		}
	}

	grab_header_info(hdBuffer);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to change the font on an array of buttons. Requires a font name, the buttons
--		to be chanaged handles, the number of buttons and the parent window.
----------------------------------------------------------------------------------------------------------------------*/
void grab_header_info(char * hdBuffer){

	char psize[100], tBytes[100], mode[100], pcks[100];

	int i = 0, p = 0, j = 0, q = 0, r = 0;

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

	SocketInfo->total_size = atoi(tBytes);
	SocketInfo->packet_size = atoi(psize);
	SocketInfo->mode = atoi(mode);
	SocketInfo->packets = atoi(pcks);
}