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
#define PORTC 55011;
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

	if ((st->client_socket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		activity("Failed to create client socket.\n", EB_STATUSBOX);
		return;
	}

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = PORTC;

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
void write_data(HWND hwnd, WPARAM wParam, LPARAM lParam){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	FILE *filept;
	errno_t fd;
	HANDLE hf = grab_file(hwnd);
	int packetsizes[] = { 256, 512, 1024, 2048 };

	int status = TransmitFile(st->client_socket, hf, 0, packetsizes[st->packet_size], 0, 0, 0);

	if (WSAGetLastError() == WSAECONNABORTED){
		activity("Failed to transmit, connection aborted.\n", EB_STATUSBOX);
	}
	
	CloseHandle(hf);
}

void disconnect(HWND hwnd){
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	closesocket(st->client_socket);
	closesocket(st->server_socket);
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