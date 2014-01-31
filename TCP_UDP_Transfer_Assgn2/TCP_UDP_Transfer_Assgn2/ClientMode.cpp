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
		MessageBox(hwnd, "WSAStartup failed on client", "Error", MB_ICONEXCLAMATION);
		return;
	}

	if ((st->client_socket = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		MessageBox(hwnd, "socket() failed on client", "Error", MB_ICONEXCLAMATION);
		return;
	}

	WSAAsyncSelect(st->client_socket, hwnd, WM_SOCKET, FD_WRITE | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = PORTC;

	if (bind(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR){
		MessageBox(hwnd, "bind failed() on client", "Error", MB_ICONEXCLAMATION);
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
	
	if (st->save_location == "NOTSET"){
		MessageBox(hwnd, "Please select a file to send.", "Error", MB_ICONEXCLAMATION);
		return -5;
	}

	memset((char *)&InternetAddr, 0, sizeof(SOCKADDR_IN));

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_port = htons(atoi(st->client_port));
	
	if ((hp = gethostbyname(st->client_send_ip)) == NULL)
	{
		MessageBox(hwnd, "Unknown server address.", "Error", MB_ICONEXCLAMATION);
		return 0;
	}

	memcpy((char *)&InternetAddr.sin_addr, hp->h_addr, hp->h_length);

	iRc = connect(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr));
	if (iRc == 0){
		MessageBox(hwnd, "Can't connect to server.", "Error", MB_ICONEXCLAMATION);
		return 0;
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
	
	HANDLE fd = CreateFile(st->save_location, GENERIC_READ, FILE_SHARE_READ,
		0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (fd == INVALID_HANDLE_VALUE){
		MessageBox(hwnd, "Unable to open file.", "Error", MB_ICONEXCLAMATION);
	}

	int status = TransmitFile(st->client_socket, fd, 0, 1024, 0, 0, 0);
	if (WSAGetLastError() == WSAECONNABORTED){
		MessageBox(hwnd, "Failed to transmit.", "Error", MB_ICONEXCLAMATION);
	}
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