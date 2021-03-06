#include <Windows.h>
#define IDC_START 0x030
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);

/*
	By Ramzi Chennafi, A00825005 3O
	Simple window containing a button. Pressing the button
	signals Extreme Racing to start.
*/
static int cxClient = 300, cyClient = 200;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Start Window 2") ;
	HWND         hwnd;
	MSG          msg;
	WNDCLASS     wndclass;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW ;
	wndclass.lpfnWndProc   = WndProc ;
	wndclass.cbClsExtra    = 0 ;
	wndclass.cbWndExtra    = 0 ;
	wndclass.hInstance     = hInstance ;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION) ;
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW) ;
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH) ;
	wndclass.lpszMenuName  = NULL ;
	wndclass.lpszClassName = szAppName ;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("This program requires Windows NT!"), 
			szAppName, MB_ICONERROR) ;
		return 0 ;
	}

	hwnd = CreateWindow (szAppName,               
		TEXT ("Start Window 2"), 
		WS_OVERLAPPEDWINDOW,        
		CW_USEDEFAULT, CW_USEDEFAULT,              
		300, 200,              
		NULL,                       
		NULL,                       
		hInstance,                  
		NULL) ;                     

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND	button;
	HDC		hdc;
	HANDLE	begin	= CreateEvent(NULL, FALSE, FALSE, TEXT("start race 2"));
	HANDLE	quit[]	= {CreateEvent(NULL, FALSE, FALSE, TEXT("race exit 2"))};
	DWORD	dwRes	= 0;
	
	dwRes = MsgWaitForMultipleObjects(1, quit, false, 0, QS_ALLINPUT);
	switch(dwRes){
		case WAIT_OBJECT_0:
			PostQuitMessage(0);
			DestroyWindow(hwnd);
	}

	switch (message){

		case WM_CREATE:
			button = CreateWindow( TEXT("button"), TEXT("Start"), 
						WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
						50, 50, 150, 80, hwnd, 
						(HMENU) IDC_START,
						((LPCREATESTRUCT) lParam)->hInstance,
						NULL);
			return 0;
		case WM_COMMAND:
			switch(LOWORD(wParam))
			{
				case IDC_START:
					SetEvent(begin);
			}
			return 0;

		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			return 0;
		
		case WM_DESTROY:
			PostQuitMessage (0);
			return 0;
	}
		return DefWindowProc (hwnd, message, wParam, lParam);
}