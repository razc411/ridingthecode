//-------------------------------------------------------------------------------------------------------
//	PROJECT: TCP/IP Resolver
//	FILE: Assignment1Term4_v2.cpp
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	FUNCTIONS: 
//			int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
//				_In_opt_ HINSTANCE hPrevInstance,
//				_In_ LPTSTR    lpCmdLine,
//				_In_ int       nCmdShow)
//			LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//			ATOM MyRegisterClass(HINSTANCE hInstance)
//			BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//			void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
//			void Main_OnPaint(HWND hwnd)
//			BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
//			void Main_OnDestroy(HWND hwnd)
//			INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//			
//	DESCRIPTION:
//			Creates the main program window, calls the message loop and sets up the various GUI aspects.
//			Also responds when corresponding resolvers are called. All responses are directed to the text box.
//	NOTES:
//			Requires WS2_32.lib for winsock. Character set must be "not set", TCHAR -> char* conversion 
//			problems will arise.
//-------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
// Global Variables:
#define MAX_LOADSTRING 100
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
//				_In_opt_ HINSTANCE hPrevInstance,
//				_In_ LPTSTR    lpCmdLine,
//				_In_ int       nCmdShow)
//	RETURNS: int for use by system
//	DESCRIPTION:
//			Registers the main window class and begins the main message loop.
//-------------------------------------------------------------------------------------------------------
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPTSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	MSG msg;
	HACCEL hAccelTable;

	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_TCP_UDP_TRANSFER_ASSGN2, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_TCP_UDP_TRANSFER_ASSGN2));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return msg.wParam;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: ATOM MyRegisterClass(HINSTANCE hInstance)
//	RETURNS: ATOM value (unused)
//	DESCRIPTION:
//			Registers the main window class;
//-------------------------------------------------------------------------------------------------------
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TCP_UDP_TRANSFER_ASSGN2));
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = CreateSolidBrush(RGB(60, 60, 60));
	wcex.lpszMenuName = MAKEINTRESOURCE(IDC_TCP_UDP_TRANSFER_ASSGN2);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
//	RETURNS: BOOL, returns false on failure of window creation
//	DESCRIPTION:
//			Intiates the window.
//-------------------------------------------------------------------------------------------------------
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 940, 765, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
//	RETURNS: LRESULT, returned to system
//	DESCRIPTION:
//			Handles window system messages. Handles WM_CREATE, WM_COMMAND, WM_PAINT AND WM_DESTROY.
//	NOTES: 
//			Uses message crackers.
//-------------------------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		HANDLE_MSG(hWnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, Main_OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, Main_OnDestroy);
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
//	RETURNS: BOOL, used by the OS
//	DESCRIPTION:
//			Creates the GUI aspects of the program on startup.
//-------------------------------------------------------------------------------------------------------
BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct){
	return TRUE;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
//	RETURNS: Nothing
//	DESCRIPTION:
//			Deals with menu items and handles messages that are sent by the "OK" buttons.
//	NOTES: 
//			OK buttons cause the respective resolution relevant to that button.
//-------------------------------------------------------------------------------------------------------
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	switch (id)
	{
	case ID_CONNECT_SERVERMODE:
		break;
	
	case ID_CONNECT_CLIENTMODE:
		break;
	
	case ID_HELP_README:
		break;

	case ID_HELP_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;

	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	}
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: void Main_OnPaint(HWND hwnd)
//	RETURNS: Nothing
//	DESCRIPTION:
//			Handles painting of the screen on WM_PAINT.
//	NOTES: 
//			Calls DrawTitleText() to draw all the visible text on the screen.
//-------------------------------------------------------------------------------------------------------
void Main_OnPaint(HWND hwnd){
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	EndPaint(hwnd, &ps);
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: void Main_OnDestroy(HWND hwnd)
//	RETURNS: Nothing
//	DESCRIPTION:
//			Exits the program, nessecary for message crackers.
//-------------------------------------------------------------------------------------------------------
void Main_OnDestroy(HWND hwnd){
	PostQuitMessage(0);
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 10 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
//	RETURNS: INT_PTR, used by OS
//	DESCRIPTION:
//			Message loop for the "About" dialog box.
//-------------------------------------------------------------------------------------------------------
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}