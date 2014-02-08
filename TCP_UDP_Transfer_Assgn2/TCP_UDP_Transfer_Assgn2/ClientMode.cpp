/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: TCP_UDP_Transfer_Assgn2
--
--	PROGRAM : A TCP/UDP File Transfer Server/Client
--
--	FUNCTIONS :
--		void init_client(HWND hwnd)
--		int client_connect(HWND hwnd)
--		void init_tcp_transfer(HWND hwnd)
--		void tcp_deliver_packets(WSABUF * wsaBuffers, SOCKET sock, int totalBytesRead, int packet_size, int buffer_count, int mode)
--		void udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, char ** buffers)
--
--	DATE: Febuary 6, 2014
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
		st->client_socket = socket(AF_INET, SOCK_DGRAM, 0);
		break;
	}

	if(st->client_socket == INVALID_SOCKET){
		activity("Failed to create client socket.\n", EB_STATUSBOX);
	}

	if (st->protocol == UDP){
		return;
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORT);

	if (bind(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR){
		activity("bind() failed on client.\n", EB_STATUSBOX);
	}

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
int client_connect(HWND hwnd){
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
		return 0;
	}

	memcpy((char *)&InternetAddr.sin_addr, hp->h_addr, hp->h_length);

	iRc = connect(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
	if (iRc != 0){
		activity("Failed to connect to server.\n", EB_STATUSBOX);
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
void init_tcp_transfer(HWND hwnd){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	int status;
	char msg[MAX_SIZE];
	HANDLE hf;
	DWORD numBytesRead = 0;
	int  totalBytesRead = 0;
	DWORD packetsizes[] = {256, 512, 1024, 2048, 5096, 10192};
	const int packet_size = packetsizes[st->packet_size];
	
	if (st->mode == 0){
		grab_file(hwnd, &hf);
	}
	activity("Sending data...\n", EB_STATUSBOX);
	WSABUF * wsaBuffers = (LPWSABUF)malloc(sizeof(WSABUF)* MAX_PACKETS);

	char * buff = (char*)malloc(sizeof(char)* packet_size);

	int i = 0, buffer_count = 0;
	while (1){
		if (st->mode == 0){
			status = ReadFile(hf, buff, packet_size, &numBytesRead, NULL);
			if (numBytesRead == 0){
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
				wsaBuffers[i].buf = buff;
				wsaBuffers[i++].len = packet_size;
			}
			totalBytesRead = packet_size * buffer_count;
			break;
		}
	}
	
	sprintf_s(msg, "Sending %d bytes in %d packets.\n", totalBytesRead, buffer_count);
	activity(msg, EB_STATUSBOX);
	tcp_deliver_packets(wsaBuffers, st->client_socket, totalBytesRead, packet_size, buffer_count, st->mode);
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
void tcp_deliver_packets(WSABUF * wsaBuffers, SOCKET sock, int totalBytesRead, int packet_size, int buffer_count, int mode){

	
	int status;
	char msg[MAX_SIZE];
	DWORD  numBytesSent = 0;
	
	char flags[HEADER_SIZE];
	memset(flags, '\0', HEADER_SIZE);
	sprintf_s(flags, "%d,%d,%d,%d;", totalBytesRead, packet_size, buffer_count, mode);
	
	LPWSABUF wsaHeaderBuf = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaHeaderBuf->len = HEADER_SIZE;
	wsaHeaderBuf->buf = flags;

	if ((status = WSASend(sock, wsaHeaderBuf, 1, &numBytesSent, NULL, NULL, NULL)) <= 0){
		sprintf_s(msg, "Error %d in TCP WSASend(header) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
		return;
	}

	for (int p = 0; p < buffer_count; p++){
		if ((status = WSASend(sock, &wsaBuffers[p], buffer_count, &numBytesSent, NULL, NULL, NULL) <= 0)){
			sprintf_s(msg, "Error %d in TCP WSASend(buffer) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return;
		}
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
void init_udp_transfer(HWND hwnd){
	
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	
	int status = 0, totalBytesRead = 0;
	char msg[MAX_SIZE];
	HANDLE hf;
	DWORD numBytesRead = 0, numBytesSent = 0;
	
	DWORD packetsizes[] = { 256, 512, 1024, 2048, 5096, 10192 };
	const int packet_size = packetsizes[st->packet_size];

	if (st->mode == 0){
		grab_file(hwnd, &hf);
	}

	char ** buffers = (char**)malloc(sizeof(char*)* MAX_PACKETS);
	char * buff = (char*)malloc(sizeof(char)* packet_size);

	int i = 0, buffer_count = 0;
	while (1){
		if (st->mode == 0){
			status = ReadFile(hf, buff, packet_size, &numBytesRead, NULL);
			if (numBytesRead == 0){
				CloseHandle(hf);
				break;
			}
			buffers[i] = buff;
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
				buffers[p] = buff;
			}
			totalBytesRead = packet_size * buffer_count;
			break;
		}
	}
	
	sprintf_s(msg, "Sending %d bytes in %d packets.\n", totalBytesRead, buffer_count);
	activity(msg, EB_STATUSBOX);

	udp_deliver_packets(hwnd, totalBytesRead, packet_size, buffer_count, buffers);
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
void udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, char ** buffers){
	
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	struct sockaddr_in sin;
	int status;
	char msg[MAX_SIZE];

	char flags[HEADER_SIZE];
	memset(flags, '\0', HEADER_SIZE);
	sprintf_s(flags, "%d,%d,%d,%d;", totalBytesRead, packet_size, buffer_count, st->mode);

	memset(&sin, 0, sizeof(sin));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(atoi(st->client_port));

	if ((sin.sin_addr.s_addr = inet_addr(st->client_send_ip)) == INADDR_NONE)
	{
		activity("failed to find address", EB_STATUSBOX);
		return;
	}
	
	if (status = setsockopt(st->client_socket, SOL_SOCKET, SO_SNDBUF, flags, sizeof(flags)) != 0)
	{
		sprintf_s(msg, "Error %d in UDP setsockopt(flags) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
		return;
	}

	if ((status = sendto(st->client_socket, flags, HEADER_SIZE, 0, (struct sockaddr *)&sin, sizeof(sin))) <= 0){
		sprintf_s(msg, "Error %d in sendto(flags) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
		return;
	}

	if (status = setsockopt(st->client_socket, SOL_SOCKET, SO_SNDBUF, buffers[0], sizeof(buffers[0])) != 0)
	{
		sprintf_s(msg, "Error %d in UDP setsockopt(buffer) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
		return;
	}

	for (int p = 0; p < buffer_count; p++){
		if ((status = sendto(st->client_socket, buffers[p], packet_size, 0, (struct sockaddr *)&sin, sizeof(sin))) <= 0)
		{
			sprintf_s(msg, "Error %d in sendto(buffer) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return;
		}
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
void disconnect(HWND hwnd){
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	closesocket(st->client_socket);
}