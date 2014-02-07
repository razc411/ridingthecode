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
	SOCKADDR_IN InternetAddr;
	WSADATA wsaData;

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0){
		activity("WSAStartup failed on server.\n", EB_STATUSBOX);
		return;
	}

	switch(st->protocol){
	case TCP:
		Listen = socket(PF_INET, SOCK_STREAM, 0);
		break;
	case UDP:
		Listen = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;
	}
	if(Listen == INVALID_SOCKET){
		activity("Failed to create client socket.\n", EB_STATUSBOX);
	}

	WSAAsyncSelect(Listen, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(atoi(st->server_port));

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		activity("bind() failed on server.\n", EB_STATUSBOX);
		return;
	}

	if (listen(Listen, 5))
	{
		activity("listen() failed on server.\n", EB_STATUSBOX);
		return;
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

	if (WSAGETSELECTERROR(lParam))
	{
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
			if (SocketInfo->header_received == 0){
				time(&startTime);
				process_tcp_header(hwnd, st->server_socket);
				buffer = (char*)malloc(sizeof(char)* (SocketInfo->packet_size * SocketInfo->packets));
				SocketInfo->header_received = 1;
				SocketInfo->BytesRECV = 0;
				SocketInfo->totalRecv = 0;
			}
			else{
				if (read_server_data(hwnd, wParam) == 1){
					break;
				}
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
void accept_data(HWND hwnd, WPARAM wParam){
	SOCKET Accept;
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	
	if ((SocketInfo = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL){
		activity("GlobalAlloc() failed with error\n", EB_STATUSBOX);
		return;
	}

	SocketInfo->header_received = 0;

	if ((st->server_socket = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
	{
		activity("Failed to accept() connection.\n", EB_STATUSBOX);
		return;
	}

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
int read_server_data(HWND hwnd, WPARAM wParam){

	DWORD Flags = 0;
	double seconds;
	char msg[MAX_SIZE];
	time_t endTime;
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	
	char * tempBuffer = (char*)malloc(sizeof(char)* SocketInfo->packet_size);
	SocketInfo->DataBuf.len = SocketInfo->packet_size;
	SocketInfo->DataBuf.buf = tempBuffer;

	if (WSARecv(st->server_socket, &SocketInfo->DataBuf, 1, &SocketInfo->BytesRECV, &Flags, NULL, NULL) == SOCKET_ERROR){
		if (WSAGetLastError() != WSAEWOULDBLOCK){
			activity("WSARecv() failed.\n", EB_STATUSBOX);
			closesocket(st->server_socket);
			return -1;
		}
		return -1;
	}

	if (st->mode == 0){
		SocketInfo->DataBuf.buf[SocketInfo->BytesRECV - 1] = '\0';
		strcat_s(buffer + SocketInfo->totalRecv, SocketInfo->BytesRECV, SocketInfo->DataBuf.buf);
	}

	SocketInfo->packets -= 1;
	SocketInfo->totalRecv += SocketInfo->BytesRECV;

	if (SocketInfo->totalRecv == SocketInfo->total_size){
		
		time(&endTime);
		seconds = difftime(endTime, startTime);

		if (st->mode == 0){
			save_file(hwnd, buffer, SocketInfo->total_size);
			sprintf_s(msg, "Recieved %d bytes. File transfer completed in %f seconds.\n", SocketInfo->totalRecv, seconds);
			activity(msg, EB_STATUSBOX);
			return 0; 
		}

		sprintf_s(msg, "Recieved %d bytes. Garbage packet transfer completed in %f seconds.\n", SocketInfo->totalRecv, seconds);
		activity(msg, EB_STATUSBOX);
		return 0; // transfer completed, closing connection
	}
	return 1; // packets remaining
}

void process_tcp_header(HWND hwnd, SOCKET recv){
	
	DWORD RecvBytes = 0;
	DWORD Flags = 0;
	char psize[100], tBytes[100], mode[100], pcks[100];
	LPWSABUF wsaBuffer = (LPWSABUF)malloc(sizeof(WSABUF));
	char * buffer = (char*)malloc(sizeof(char)* HEADER_SIZE);
	memset(buffer, 0, HEADER_SIZE);
	wsaBuffer->len = HEADER_SIZE;
	wsaBuffer->buf = buffer;

	if (WSARecv(recv, wsaBuffer, 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR){
		if (WSAGetLastError() != WSAEWOULDBLOCK){
			activity("WSARecv() failed.\n", EB_STATUSBOX);
			closesocket(recv);
		}
	}
	int i = 0, p = 0, j = 0, q = 0, r = 0;
	
	//grabs total bytes
	for (; buffer[i] != ','; i++){
		tBytes[j++] = buffer[i];
	}
	tBytes[j] = '\0';

	//grabs packet size
	for (i += 1; buffer[i] != ','; i++){
		psize[p++] = buffer[i];
	}
	psize[p] = '\0';

	//grabs total packets
	for (i += 1; buffer[i] != ','; i++){
		pcks[r++] = buffer[i];
	}
	pcks[r] = '\0';
	
	//grabs times the packets are sent
	for (i += 1; buffer[i] != ';'; i++){
		mode[q++] = buffer[i];
	}
	mode[q] = '\0';

	SocketInfo->total_size = atoi(tBytes);
	SocketInfo->packet_size = atoi(psize);
	SocketInfo->mode = atoi(mode);
	SocketInfo->packets = atoi(pcks);
}