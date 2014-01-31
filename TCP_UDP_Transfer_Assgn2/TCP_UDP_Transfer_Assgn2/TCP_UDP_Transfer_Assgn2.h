#pragma once

#include "resource.h"
#define WM_SOCKET (WM_USER + 1)
#define MAX_SIZE 1024

typedef struct _SOCKET_INFORMATION {
	BOOL RecvPosted;
	CHAR Buffer[DATA_BUFSIZE];
	WSABUF DataBuf;
	SOCKET Socket;
	DWORD BytesSEND;
	DWORD BytesRECV;
	_SOCKET_INFORMATION *Next;
} SOCKET_INFORMATION, *LPSOCKET_INFORMATION;

typedef struct _SETTINGS{
	char* server_port;
	char* client_port;
	char* client_send_ip;
	int server_prtcl;
	int client_prtcl;
	char * save_location;
	SOCKET client_socket;
}SETTINGS;

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
// WndProc called functions
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				Main_OnPaint(HWND hwnd);
void				Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void				Main_OnDestroy(HWND hwnd);
BOOL				Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
// GUI setup functions
void				DrawButtons(HWND hwndParent);
void				DrawTitleText(HDC hdc);
void				DrawDisplay(HWND hwndParent);
void				SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons);

void				Init_Settings(HWND hwnd);
INT_PTR CALLBACK	Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void				set_settings(HWND hwnd);
void				CreateSocketInformation(SOCKET s);
LPSOCKET_INFORMATION GetSocketInformation(SOCKET s);
void				FreeSocketInformation(SOCKET s);
//server testing functions
void				init_server(HWND hwnd);
int					socket_event(HWND hwnd, WPARAM wParam, LPARAM lParam);
int					read_server_data(HWND hwnd, WPARAM wParam);
SOCKET				accept_data(HWND hwnd, WPARAM wParam);

void     			init_client(HWND hwnd);
int					client_connect(HWND hwnd);
void				write_data(HWND hwnd, WPARAM wParam, LPARAM lParam);

void grab_file(HWND hwnd);