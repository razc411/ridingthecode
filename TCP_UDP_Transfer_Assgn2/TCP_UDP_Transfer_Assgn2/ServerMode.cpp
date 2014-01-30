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
LPSOCKET_INFORMATION SocketInfoList;
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

	MSG msg;
	DWORD Ret;
	SOCKET Listen;
	SOCKADDR_IN InternetAddr;
	WSADATA wsaData;

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0){
		MessageBox(hwnd, "WSAStartup failed on server", "Error", MB_ICONEXCLAMATION);
		return;
	}

	if ((Listen = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		MessageBox(hwnd, "socket() failed on server", "Error", MB_ICONEXCLAMATION);
		return;
	}

	WSAAsyncSelect(Listen, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(atoi(st->server_port));

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		MessageBox(hwnd, "bind() failed on server", "Error", MB_ICONEXCLAMATION);
		return;
	}

	if (listen(Listen, 5))
	{
		MessageBox(hwnd, "listen() failed on server", "Error", MB_ICONEXCLAMATION);
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

	if (WSAGETSELECTERROR(lParam))
	{
		//printf("Socket failed with error %d\n", WSAGETSELECTERROR(lParam));
		FreeSocketInformation(wParam);
	}
	else
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:
			if (accept_data(hwnd, wParam))
				return -4;
			break;
		case FD_WRITE:
			write_data(hwnd, wParam, lParam);
			break;
		case FD_READ:
			if (read_server_data(hwnd, wParam))
				return -3;
			break;

		case FD_CLOSE:
			FreeSocketInformation(wParam);
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
SOCKET accept_data(HWND hwnd, WPARAM wParam){
	SOCKET Accept;

	if ((Accept = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
	{
		MessageBox(hwnd, "accept() failed on server", "Error", MB_ICONEXCLAMATION);
		return Accept;
	}

	// Create a socket information structure to associate with the
	// socket for processing I/O.

	CreateSocketInformation(Accept);

	WSAAsyncSelect(Accept, hwnd, WM_SOCKET, FD_READ | FD_CLOSE);

	return Accept;
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

	LPSOCKET_INFORMATION SocketInfo = GetSocketInformation(wParam);
	DWORD RecvBytes;
	DWORD Flags;

	if (SocketInfo->BytesRECV != 0)
	{
		SocketInfo->RecvPosted = TRUE;
		return 0;
	}
	else
	{
		SocketInfo->DataBuf.buf = SocketInfo->Buffer;
		SocketInfo->DataBuf.len = DATA_BUFSIZE;

		Flags = 0;
		if (WSARecv(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &RecvBytes,
			&Flags, NULL, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				MessageBox(hwnd, "WSARecv() failed on server", "Error", MB_ICONEXCLAMATION);
				FreeSocketInformation(wParam);
				return 0;
			}
		}
		else // No error so update the byte count
		{
			SocketInfo->BytesRECV = RecvBytes;
		}
		SendMessage(GetDlgItem(hwnd, EB_STATBOX), WM_SETTEXT, NULL, (LPARAM)SocketInfo->Buffer);
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
void CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR, sizeof(SOCKET_INFORMATION))) == NULL){
		MessageBox(GetActiveWindow(), "GlobalAlloc() failed on server", "Error", MB_ICONEXCLAMATION);
		return;
	}

	SI->Socket = s;
	SI->RecvPosted = FALSE;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SI->Next = SocketInfoList;

	SocketInfoList = SI;
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
LPSOCKET_INFORMATION GetSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION *SI = SocketInfoList;

	while (SI)
	{
		if (SI->Socket == s)
			return SI;

		SI = SI->Next;
	}

	return NULL;
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
void FreeSocketInformation(SOCKET s)
{
	SOCKET_INFORMATION *SI = SocketInfoList;
	SOCKET_INFORMATION *PrevSI = NULL;

	while (SI)
	{
		if (SI->Socket == s)
		{
			if (PrevSI)
				PrevSI->Next = SI->Next;
			else
				SocketInfoList = SI->Next;

			closesocket(SI->Socket);
			GlobalFree(SI);
			return;
		}

		PrevSI = SI;
		SI = SI->Next;
	}
}
