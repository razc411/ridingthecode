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
		st->client_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		break;
	}
	if(st->client_socket == INVALID_SOCKET){
		activity("Failed to create client socket.\n", EB_STATUSBOX);
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORTC);

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
void write_client_data(HWND hwnd, WPARAM wParam, LPARAM lParam){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	int status = 1;
	HANDLE hf;
	LPSTR title;
	DWORD numBytesRead = 0, numBytesSent = 0;
	int  totalBytesRead = 0;
	DWORD packetsizes[] = { 5096, 256, 512, 1024, 2048 };
	const int packet_size = packetsizes[st->packet_size];
	
	if (st->mode == 0){
		title = grab_file(hwnd, &hf);
	}

	WSABUF wsaBuffers[MAX_PACKETS];
	LPWSABUF wsaHeaderBuf = (LPWSABUF)malloc(sizeof(WSABUF));
	wsaHeaderBuf->len = HEADER_SIZE;

	char * buff = (char*)malloc(sizeof(char)* packet_size);
	memset(buff, 0, packet_size);

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
			for (int p = 0; p < buffer_count; p++){
				for (int g = 0; g < packet_size; g++){
					buff[g] = 'p';
				}
				buff[packet_size - 1] = '\0';
				wsaBuffers[i].buf = buff;
				wsaBuffers[i++].len = packet_size;
			}
			totalBytesRead = packet_size * buffer_count;
			break;
		}
	}

	char flags[HEADER_SIZE];
	memset(flags, '\0', HEADER_SIZE);
	sprintf_s(flags, "%d,%d,%d,%d;", totalBytesRead, packet_size, buffer_count, st->mode);
	wsaHeaderBuf->buf = flags;

	WSASend(st->client_socket, wsaHeaderBuf, 1, &numBytesSent, NULL, NULL, NULL);
	
	for (int p = 0; p < buffer_count; p++){
		WSASend(st->client_socket, &wsaBuffers[p], buffer_count, &numBytesSent, NULL, NULL, NULL);
	}
}


void disconnect(HWND hwnd){
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	closesocket(st->client_socket);
}


//UDP TRANSFER
//char pkt[...];
//size_t pkt_length = ...;
//sockaddr_in dest;
//sockaddr_in local;
//WSAData data;
//WSAStartup(MAKEWORD(2, 2), &data);
//
//local.sin_family = AF_INET;
//local.sin_addr.s_addr = inet_addr(<source IP address>);
//local.sin_port = 0; // choose any
//
//dest.sin_family = AF_INET;
//dest.sin_addr.s_addr = inet_addr(<destination IP address>);
//dest.sin_port = htons(<destination port number>);
//
//// create the socket
//SOCKET s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
//// bind to the local address
//bind(s, (sockaddr *)&local, sizeof(local));
//// send the pkt
//int ret = sendto(s, pkt, pkt_length, 0, (sockaddr *)&dest, sizeof(dest));