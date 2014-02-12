#pragma once

#include "resource.h"

typedef struct _SETTINGS{
	char* server_port;
	char* client_port;
	char* client_send_ip;
	SOCKET server_socket;
	int protocol;
	int packet_size;
	int mode;
	char* times_to_send;
	SOCKET client_socket;
}SETTINGS;

typedef struct _SOCKET_INFORMATION {
	WSABUF DataBuf;
	DWORD BytesRECV;
	int initPackets;
	int packets;
	int packet_size;
	int total_size;
	int mode;
	int header_received;
	int totalRecv;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

// Procs
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
// Program Intiation
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
void				Init_Settings(HWND hwnd);
// WndProc called functions
void				Main_OnPaint(HWND hwnd);
void				Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void				Main_OnDestroy(HWND hwnd);
BOOL				Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
// GUI setup functions
void				DrawButtons(HWND hwndParent);
void				DrawTitleText(HDC hdc);
void				DrawDisplay(HWND hwndParent);
void				SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons);
// Connection Establishment
// Server
void				init_server(HWND hwnd);
void				socket_event(HWND hwnd, WPARAM wParam, LPARAM lParam);
void				accept_data(HWND hwnd, WPARAM wParam);
// Client
void     			init_client(HWND hwnd);
void				client_connect(HWND hwnd);
// UDP Client Side
void				udp_deliver_packets(HWND hwnd, int totalBytesRead, int packet_size, int buffer_count, WSABUF * wsaBuffers);
// TCP Client Side
void				init_transfer(HWND hwnd);
void				tcp_deliver_packets(HWND hwnd, WSABUF * wsaBuffers, SOCKET sock, int totalBytesRead, int packet_size, int buffer_count, int mode);
// TCP Server Side
int					read_tcp(HWND hwnd, SOCKET sock);
int					init_tcp_receive(HWND hwnd);
// UDP Server Side
int					read_udp(HWND hwnd, SOCKET sock);
int					init_udp_receive(HWND hwnd);
int					udp_transfer_completion(HWND hwnd, int mode);
// Both protocols, server side
int					transfer_completion(HWND hwnd, int mode);
int					process_header(HWND hwnd, SOCKET recv);
void				grab_header_info(char * hdBuffer);
int					receive_acknowledge(HWND hwnd);
void				acknowledge(HWND hwnd);
// Utility functions
void				set_settings(HWND hwnd);
LPSTR				grab_file(HWND hwnd, HANDLE * hf);
void				save_file(HWND hwnd, char * buffer, int size);
void				activity(char * buffer, int box);
void				disconnect(HWND hwnd);
int					grab_header(HWND hwnd, SOCKET sock);
void				set_transfer_stats_server(HWND hwnd, LPSOCKET_INFORMATION SocketInfo, double seconds);
void				set_transfer_stats_client(HWND hwnd, int lost_packets, int total_size, double seconds, int packet_size);
