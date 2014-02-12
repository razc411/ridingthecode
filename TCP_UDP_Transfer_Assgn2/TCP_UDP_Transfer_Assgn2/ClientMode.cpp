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
		return -1;
	}

	setsockopt(st->client_socket, SOL_SOCKET, SO_REUSEADDR, 0, 0);
	SetClassLongPtr(hwnd, 0, (LONG)st);
	activity("Connected to server.\n", EB_STATUSBOX);
	
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
void init_transfer(HWND hwnd){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	int status;
	char msg[MAX_SIZE];
	HANDLE hf;
	DWORD numBytesRead = 0;
	int  totalBytesRead = 0;
	DWORD packetsizes[] = {1024, 4096, 20000, 60000};
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
		tcp_deliver_packets(wsaBuffers, st->client_socket, totalBytesRead, packet_size, buffer_count, st->mode);

	free(wsaBuffers);
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
	sprintf_s(flags, ";%d,%d,%d,%d;", totalBytesRead, packet_size, buffer_count, mode);
	
	LPWSABUF wsaHeaderBuf = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaHeaderBuf->len = HEADER_SIZE;
	wsaHeaderBuf->buf = flags;

	if ((status = WSASend(sock, wsaHeaderBuf, 1, &numBytesSent, NULL, NULL, NULL)) < 0){
		sprintf_s(msg, "Error %d in TCP WSASend(header) with return of %d\n", WSAGetLastError(), status);
		activity(msg, EB_STATUSBOX);
	}

	for (int p = 0; p < buffer_count; p++){
		if ((status = WSASend(sock, &wsaBuffers[p], 1, &numBytesSent, NULL, NULL, NULL)) < 0){
			sprintf_s(msg, "Error %d in TCP WSASend(buffer) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
			return;
		}
	}

	activity("Data transmission complete.\n", EB_STATUSBOX);
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
void udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, WSABUF * buffers){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

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
	
	while (1){
		if ((status = WSASendTo(st->client_socket, wsaHeaderBuf, 1, &numBytesSent, 0, (struct sockaddr *)&sin, sizeof(sin), NULL, NULL)) < 0){
			sprintf_s(msg, "Error %d in TCP WSASend(header) with return of %d\n", WSAGetLastError(), status);
			activity(msg, EB_STATUSBOX);
		}

		if (receive_acknowledge(hwnd) == 1) { break; }
	}
		
	for (int p = 0; p < buffer_count; p++){
		while (1){
			if ((status = WSASendTo(st->client_socket, &buffers[p], 1, &numBytesSent, 0, (struct sockaddr *)&sin, sizeof(sin), NULL, NULL)) < 0){
				sprintf_s(msg, "Error %d in sendto(buffer) with return of %d\n", WSAGetLastError(), status);
				activity(msg, EB_STATUSBOX);
				return;
			}

			if (receive_acknowledge(hwnd) == 1){ break; }
		}
	}

	activity("Data transmission complete.", EB_STATUSBOX);
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