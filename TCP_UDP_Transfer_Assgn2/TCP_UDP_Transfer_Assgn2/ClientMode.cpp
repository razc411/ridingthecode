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

void init_client(HWND hwnd){
	MSG msg;
	DWORD Ret;
	SOCKET Send;
	SOCKADDR_IN InternetAddr;
	WSADATA wsaData;

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	if ((Ret = WSAStartup(0x0202, &wsaData)) != 0){
		return;
	}

	if ((Send = socket(PF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		printf("socket() failed with error %d\n", WSAGetLastError());
		return;
	}

	WSAAsyncSelect(Send, hwnd, WM_SOCKET, FD_WRITE | FD_CLOSE);

	InternetAddr.sin_family = AF_INET;
	InternetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	InternetAddr.sin_port = htons(st->client_port);

	if (bind(Send, (PSOCKADDR)&InternetAddr, sizeof(InternetAddr)) == SOCKET_ERROR)
	{
		printf("bind() failed with error %d\n", WSAGetLastError());
		return;
	}
}

void write_data(HWND hwnd, WPARAM wParam, int fd){

	DWORD SendBytes;
	DWORD Flags;
	LPSOCKET_INFORMATION SocketInfo = GetSocketInformation(wParam);
	WSABUF buffer = { 9, "testing" };

	if (SocketInfo->BytesRECV > SocketInfo->BytesSEND)
	{
		SocketInfo->DataBuf.buf = SocketInfo->Buffer + SocketInfo->BytesSEND;
		SocketInfo->DataBuf.len = SocketInfo->BytesRECV - SocketInfo->BytesSEND;

		if (WSASend(SocketInfo->Socket, &buffer, 1, &SendBytes, 0, NULL, NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != WSAEWOULDBLOCK)
			{
				FreeSocketInformation(wParam);
				return;
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

void send_data(){

}

