// Contains all function defintions
#pragma once
#include "resource.h"
// Main window functions
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
HWND				Create_Ok_Button(HWND hwnd, int xcoord, int ycoord, HINSTANCE hInst, int name);
HWND				Create_Edit_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name);
HWND				Create_CB_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name);
HWND				Create_Output_Box(HWND hwndParent, HINSTANCE hInst);
HWND				Create_Clear_Button(HWND hwnd, HINSTANCE hInst);
void				DrawTitleText(HDC hdc);
// TCP/IP Resolution functions
int					resolve_IP(HWND hwnd);
int					resolve_host(HWND hwnd);
int					resolve_service(HWND hwnd);
int					resolve_port(HWND hwnd);
void				StartWinsock();
int					getProtocol(HWND protocl_cb, TCHAR * prtclBuff);
void				phosterr(HWND text_box);