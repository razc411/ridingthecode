/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: TCP_UDP_Transfer_Assign2
--
--	PROGRAM : File/Packet transfer through UDP and TCP.
--
--	FUNCTIONS :
--		int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
--		ATOM MyRegisterClass(HINSTANCE hInstance)
--		BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
--		BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
--		void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
--		void Main_OnPaint(HWND hwnd)
--		void Main_OnDestroy(HWND hwnd)
--		INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
--		INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
--		void disconnect(HWND hwnd)
--
--	DATE: Febuary 5 2014
--	REVISIONS : None
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	DESCRIPTION:
--		A UDP/TCP File/packet transfering program. User starts up the client on two computers, one in server and the other in client.
--		These two servers are now able to send entire data files through UDP or TCP, or are able to send garbage packets at the specified amount
--		and size by the user, filled with the letter 'p'. 
--	NOTES:
--		- Maximum transfer size = 2000000 * 60KB = 120 Gigabytes. Untested on files above 1 Gigabyte. Expect huge slowdown transfering large files with UDP.
--		- UDP transfer is somewhat reliable, and will resend dropped packets. Gives no consideration for pack order, which shouldn't be an issue on an uncrowded LAN.
--		- Both clients need the ip of the other client put into the settings. 
--		- The client socket on the client side must be set to the same port as the server port on the server side. The same can be said for the server port on the
--		the client side and the client port on the server side.
--		- On a TCP connection, after both the server and client have been intialized, the client must "Connect" with the server. This can be done using the obvious button.
--		- UDP does not require the user to "Connect". Simply intialize each side.
--		- Program is single threaded and asynchronous. Spent too much time on other considerations. It will hang while transferring data.
--		- Only tested on filesizes up to 600 MB.
--		- The packet size setting will also affect the packet size of file transfers, and not just garbage transfers.
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"

// Global Variables:
#define MAX_LOADSTRING 100
HINSTANCE hInst;								
TCHAR szTitle[MAX_LOADSTRING];					
TCHAR szWindowClass[MAX_LOADSTRING];			
static int mode;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: _tWinMain
--
--      DATE: Febuary 5 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: int APIENTRY _tWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPTSTR    lpCmdLine, _In_ int       nCmdShow)
--
--      RETURNS: int (for OS purposes).
--
--      NOTES:
--      Program entry point. Same old intialization as every other win32 c program.
----------------------------------------------------------------------------------------------------------------------*/
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
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = sizeof(SETTINGS) + (sizeof(char) * MAX_SIZE);
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
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	HWND hWnd;

	hInst = hInstance;

	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 940, 600, NULL, NULL, hInstance, NULL);

	if (!hWnd)
		return FALSE;
	 
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{

	switch (message){
		HANDLE_MSG(hWnd, WM_CREATE, Main_OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, Main_OnCommand);
		HANDLE_MSG(hWnd, WM_PAINT, Main_OnPaint);
		HANDLE_MSG(hWnd, WM_DESTROY, Main_OnDestroy);
		case WM_SOCKET:
			socket_event(hWnd, wParam, lParam);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
			
	}

	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
BOOL Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct){
	
	SETTINGS * st = (SETTINGS*) malloc(sizeof(SETTINGS));
	st->client_port = "5150";
	st->server_port = "5151";
	st->protocol = 0;
	st->packet_size = 2;
	st->times_to_send = "100";
	st->client_send_ip = "127.0.0.1";
	st->mode = 1;
	SetClassLongPtr(hwnd, 0, (LONG)st);

	DrawDisplay(hwnd);
	DrawButtons(hwnd);
	UpdateWindow(hwnd);
	
	return TRUE;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
void Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify){
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	char * msg;

	switch (id){
	case BT_SEND:
		init_transfer(hwnd);
		break;
	case BT_DISCONNECT:
		disconnect(hwnd);
		break;
	case ID_TRANSFER_MODE:
		st->mode = (st->mode == 0) ? 1 : 0;
		msg = (st->mode == 0) ? "File transfer mode activated.\n" : "Packet transfer mode activated.\n";
		activity(msg, EB_STATUSBOX);
		SetClassLongPtr(hwnd, 0, (LONG)st);
		break;
	case BT_CONNECT:
		client_connect(hwnd);
		break;
	case ID_FILE_SETTINGS:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_SETTINGS), hwnd, Settings);
		break;
	case ID_CONNECT_SERVERMODE:
		init_server(hwnd);
		mode = SERVER;
		break;
	case ID_CONNECT_CLIENTMODE:
		init_client(hwnd);
		mode = CLIENT;
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
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
void Main_OnPaint(HWND hwnd){
	PAINTSTRUCT ps;
	HDC hdc = BeginPaint(hwnd, &ps);
	DrawTitleText(hdc);
	EndPaint(hwnd, &ps);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
void Main_OnDestroy(HWND hwnd){
	SETTINGS * st = (SETTINGS*) GetClassLongPtr(hwnd, 0);
	closesocket(st->client_socket);
	closesocket(st->server_socket);
	PostQuitMessage(0);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL){
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: main
--
--      DATE: January 20, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void main(void)
--
--      RETURNS: void
--
--      NOTES:
--      Program entry point. Disables terminal processing, creates 3 pipes and 2 children processes. These processes
--		are directed into their respective function paths.
----------------------------------------------------------------------------------------------------------------------*/
INT_PTR CALLBACK Settings(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam){
	switch (message){
	case WM_INITDIALOG:
		Init_Settings(hDlg);
		return TRUE;

	case WM_COMMAND:
		switch(LOWORD(wParam)){
		case BT_SETTINGCANCEL:
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		case BT_SETTINGOK:
			set_settings(hDlg);			
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
	}
	return FALSE;
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
void disconnect(HWND hwnd){
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	closesocket(st->client_socket);
	closesocket(st->server_socket);
	WSACleanup();
	activity("Connection disconnected.\n", EB_STATUSBOX);
}