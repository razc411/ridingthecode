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
#define PORT 55001;
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
	SOCKET Send;
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
	InternetAddr.sin_port = PORT;

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
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	memset((char *)&InternetAddr, 0, sizeof(SOCKADDR_IN));
	int iRc;

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_port = htons(atoi(st->client_port));
	
	if ((hp = gethostbyname(st->client_send_ip)) == NULL)
	{
		MessageBox(hwnd, "Unknown server address.", "Error", MB_ICONEXCLAMATION);
		return 0;
	}

	memcpy((char *)&InternetAddr.sin_addr, hp->h_addr, hp->h_length);

	if ((iRc = connect(st->client_socket, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == -1)){
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

	DWORD SendBytes;
	DWORD Flags;
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	WSABUF buffer = { 9, "testing" };

	if (WSASend(st->client_socket, &buffer, 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			FreeSocketInformation(wParam);
			return;
		}
	}
}


//void init_client(HWND hwnd){
//	int n, ns, bytes_to_read;
//	int port, err;
//	SOCKET sd;
//	struct hostent	*hp;
//	struct sockaddr_in server;
//	char  *host, *bp, rbuf[1024], sbuf[1024] = "testing", **pptr;
//	WSADATA WSAData;
//	WORD wVersionRequested;
//	
//	set_settings(hwnd);
//	
//	SETTINGS * st = (SETTINGS*) GetClassLongPtr(hwnd, 0);
//	char* ip = ip_convert(st);
//	port = st->client_port;
//
//	wVersionRequested = MAKEWORD(2, 2);
//	err = WSAStartup(wVersionRequested, &WSAData);
//	if (err != 0) //No usable DLL
//	{
//		printf("DLL not found!\n");
//		exit(1);
//	}
//
//	// Create the socket
//	if ((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
//	{
//		perror("Cannot create socket");
//		exit(1);
//	}
//
//	// Initialize and set up the address structure
//	memset((char *)&server, 0, sizeof(struct sockaddr_in));
//	server.sin_family = AF_INET;
//	server.sin_port = htons(port);
//	if ((hp = gethostbyname(ip)) == NULL)
//	{
//		fprintf(stderr, "Unknown server address\n");
//		exit(1);
//	}
//
//	// Copy the server address
//	memcpy((char *)&server.sin_addr, hp->h_addr, hp->h_length);
//
//	// Connecting to the server
//	if (connect(sd, (struct sockaddr *)&server, sizeof(server)) == -1)
//	{
//		SendMessage(GetDlgItem(hwnd, EB_STATUSBOX) , WM_SETTEXT, NULL, (LPARAM)"Failed to connect");
//		perror("connect");
//		exit(1);
//	}
//	printf("Connected:    Server Name: %s\n", hp->h_name);
//	pptr = hp->h_addr_list;
//	printf("\t\tIP Address: %s\n", inet_ntoa(server.sin_addr));
//	printf("Transmiting:\n");
//
//	// Transmit data through the socket
//	ns = send(sd, sbuf, BUFSIZE, 0);
//	printf("Receive:\n");
//	bp = rbuf;
//	bytes_to_read = BUFSIZE;
//
//	// client makes repeated calls to recv until no more data is expected to arrive.
//    n = recv(sd, bp, bytes_to_read, 0);
//	SendMessage(GetDlgItem(hwnd, EB_STATBOX), WM_SETTEXT, NULL, (LPARAM)"Data Sent!");
//	closesocket(sd);
//	WSACleanup();
//}
