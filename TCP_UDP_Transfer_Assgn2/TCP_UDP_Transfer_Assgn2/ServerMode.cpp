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
static int header_recv, packet_size, totalBytes, timestosend, packets;
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
	header_recv = 0;

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
			if (header_recv == 0){
				process_tcp_header(hwnd, st->server_socket, &packet_size, 
					&totalBytes, &timestosend, &packets);
			}
			else{
				read_server_data(hwnd, wParam, packet_size, totalBytes, timestosend, packets);
			}

			header_recv = 1;
			break;
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
int read_server_data(HWND hwnd, WPARAM wParam, int packet_size, int totalBytes, int timestosend, int packets){

	DWORD RecvBytes = 0;
	DWORD Flags = 0;
	LPWSABUF wsaBuffer = (LPWSABUF)malloc(sizeof(WSABUF));
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	int size = 0;
	int file_size = totalBytes;
	char * file = (char*)malloc(sizeof(char)* file_size);
	memset(file, '\0', file_size + 1);

	char * buffer = (char*)malloc(sizeof(char)* packet_size);
	memset(buffer, '\0', packet_size);
	wsaBuffer->len = packet_size;
	wsaBuffer->buf = buffer;

	//issue lies here
	for (int i = 0; i < packets; i++){
		if (WSARecv(st->server_socket, wsaBuffer, 1, &RecvBytes, &Flags, NULL, NULL) == SOCKET_ERROR){
			if (WSAGetLastError() != WSAEWOULDBLOCK){
				activity("WSARecv() failed.\n", EB_STATUSBOX);
				closesocket(st->server_socket);
				return 0;
			}
		}
		strcat_s(file, wsaBuffer->len, wsaBuffer->buf);
	}

	save_file(hwnd, file, file_size);
}

void process_tcp_header(HWND hwnd, SOCKET recv, int * packet_size, int * totalBytes, int * timestosend, int * packets){
	
	DWORD RecvBytes = 0;
	DWORD Flags = 0;
	char psize[100], tBytes[100], tts[100], pcks[100];
	LPWSABUF wsaBuffer = (LPWSABUF)malloc(sizeof(WSABUF));
	char * buffer = (char*)malloc(sizeof(char)* DATA_BUFSIZE);
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
		tts[q++] = buffer[i];
	}
	tts[q] = '\0';

	*totalBytes = atoi(tBytes);
	*packet_size = atoi(psize);
	*timestosend = atoi(tts);
	*packets = atoi(pcks);
}