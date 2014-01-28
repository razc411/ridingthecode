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

void init_server(HWND hwnd){
	MSG msg;
	DWORD Ret;
	SOCKET Listen;
	SOCKADDR_IN InternetAddr;
	WSADATA wsaData;
	
	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0)
	{
		printf("WSAStartup failed with error %d\n", Ret);
		return;
	}

	if ((Listen = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("socket() failed with error %d\n", WSAGetLastError());
		return;
	}

	WSAAsyncSelect(Listen, hwnd, WM_SOCKET, FD_ACCEPT | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(PORT);

	if (bind(Listen, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error %d\n", WSAGetLastError());
		return;
	}

	if (listen(Listen, 5))
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		return;
	}
}

int socket_event(HWND hwnd, LPARAM lParam, WPARAM wParam){
	SOCKET Accept;
	LPSOCKET_INFORMATION SocketInfo;
	DWORD RecvBytes, SendBytes;
	DWORD Flags;

	if (WSAGETSELECTERROR(lParam))
	{
		printf("Socket failed with error %d\n", WSAGETSELECTERROR(lParam));
		FreeSocketInformation(wParam);
	}
	else
	{
		switch (WSAGETSELECTEVENT(lParam))
		{
		case FD_ACCEPT:

			if ((Accept = accept(wParam, NULL, NULL)) == INVALID_SOCKET)
			{
				printf("accept() failed with error %d\n", WSAGetLastError());
				break;
			}

			// Create a socket information structure to associate with the
			// socket for processing I/O.

			CreateSocketInformation(Accept);

			printf("Socket number %d connected\n", Accept);

			WSAAsyncSelect(Accept, hwnd, WM_SOCKET, FD_READ | FD_WRITE | FD_CLOSE);

			break;

		case FD_READ:

			SocketInfo = GetSocketInformation(wParam);

			// Read data only if the receive buffer is empty.

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
						printf("WSARecv() failed with error %d\n", WSAGetLastError());
						FreeSocketInformation(wParam);
						return 0;
					}
				}
				else // No error so update the byte count
				{
					SocketInfo->BytesRECV = RecvBytes;
				}
				SendMessage(GetDlgItem(hwnd, EB_STATBOX) , WM_SETTEXT, NULL, (LPARAM)SocketInfo->Buffer);
			}

			// DO NOT BREAK HERE SINCE WE GOT A SUCCESSFUL RECV. Go ahead
			// and begin writing data to the client.

		case FD_WRITE:

			SocketInfo = GetSocketInformation(wParam);

			if (SocketInfo->BytesRECV > SocketInfo->BytesSEND)
			{
				SocketInfo->DataBuf.buf = SocketInfo->Buffer + SocketInfo->BytesSEND;
				SocketInfo->DataBuf.len = SocketInfo->BytesRECV - SocketInfo->BytesSEND;

				if (WSASend(SocketInfo->Socket, &(SocketInfo->DataBuf), 1, &SendBytes, 0,
					NULL, NULL) == SOCKET_ERROR)
				{
					if (WSAGetLastError() != WSAEWOULDBLOCK)
					{
						printf("WSASend() failed with error %d\n", WSAGetLastError());
						FreeSocketInformation(wParam);
						return 0;
					}
				}
				else // No error so update the byte count
				{
					SocketInfo->BytesSEND += SendBytes;
				}
			}

			if (SocketInfo->BytesSEND == SocketInfo->BytesRECV)
			{
				SocketInfo->BytesSEND = 0;
				SocketInfo->BytesRECV = 0;

				// If a RECV occurred during our SENDs then we need to post an FD_READ
				// notification on the socket.

				if (SocketInfo->RecvPosted == TRUE)
				{
					SocketInfo->RecvPosted = FALSE;
					PostMessage(hwnd, WM_SOCKET, wParam, FD_READ);
				}
			}

			break;

		case FD_CLOSE:

			printf("Closing socket %d\n", wParam);
			FreeSocketInformation(wParam);

			break;
		}
	}
}

void recieve_data(){

}

void save_data(){

}

void CreateSocketInformation(SOCKET s)
{
	LPSOCKET_INFORMATION SI;

	if ((SI = (LPSOCKET_INFORMATION)GlobalAlloc(GPTR,
		sizeof(SOCKET_INFORMATION))) == NULL)
	{
		printf("GlobalAlloc() failed with error %d\n", GetLastError());
		return;
	}

	// Prepare SocketInfo structure for use.

	SI->Socket = s;
	SI->RecvPosted = FALSE;
	SI->BytesSEND = 0;
	SI->BytesRECV = 0;

	SI->Next = SocketInfoList;

	SocketInfoList = SI;
}

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
