/*------------------------------------------------------------
Synchro Assignment - Extreme Racing
By Ramzi Chennafi A00825005

Multithreaded car racing program. Races can be done in
synchronized(right click) and unsynchronized (left click) mode.

Requires a button press on either StartWindow 1 or StartWindow 2
to intiate the program.
------------------------------------------------------------*/
#include "resource.h"

HWND				mainHWND;
CRITICAL_SECTION	cs;
static int			cxClient	= 800, cyClient = 600;
BOOL				GLOBAL		= FALSE;
static BOOL			winner		= FALSE;
static bool			wndExit[]	= {false, false};
HANDLE				events[]	= {CreateEvent(NULL, FALSE, FALSE, TEXT("start race 1")), CreateEvent(NULL, FALSE, FALSE, TEXT("start race 2"))};

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	TCHAR*		szCaption = TEXT("WAS NOT STARTED");
	TCHAR*		szAppName = TEXT("Extreme Racing");
	DWORD		dwRes	  = 0;
	HWND		hwnd;
	MSG         msg;
	WNDCLASS    wndclass;

	wndclass.style         = CS_HREDRAW | CS_VREDRAW;
	wndclass.lpfnWndProc   = WndProc;
	wndclass.cbClsExtra    = 0;
	wndclass.cbWndExtra    = 0;
	wndclass.hInstance     = hInstance;
	wndclass.hIcon         = LoadIcon (NULL, IDI_APPLICATION);
	wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
	wndclass.hbrBackground = (HBRUSH) GetStockObject (WHITE_BRUSH);
	wndclass.lpszMenuName  = NULL;
	wndclass.lpszClassName = szAppName;

	if (!RegisterClass (&wndclass))
	{
		MessageBox (NULL, TEXT ("Shit doesn't work!"), 
			szAppName, MB_ICONERROR);
		return 0;
	}

	dwRes = WaitForMultipleObjects(2, events, FALSE, INFINITE);
	switch(dwRes){
		case WAIT_OBJECT_0:
			szCaption = TEXT("Window 1 Started Extreme Racing");
			wndExit[0] = true;
		break;
		
		case WAIT_OBJECT_0 + 1:
			szCaption = TEXT("Window 2 Started Extreme Racing");
			wndExit[1] = true;
		break;
	}

	hwnd = CreateWindow (szAppName, // window class name
		szCaption,					// window caption
		WS_OVERLAPPEDWINDOW,        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		800,						// initial x size
		600,						// initial y size
		NULL,                       // parent window handle
		NULL,                       // window menu handle
		hInstance,                  // program instance handle
		NULL) ;                     // creation parameters

	ShowWindow (hwnd, iCmdShow) ;
	UpdateWindow (hwnd) ;

	mainHWND = hwnd;

	while (GetMessage (&msg, NULL, 0, 0))
	{
		TranslateMessage (&msg) ;
		DispatchMessage (&msg) ;
	}
	return msg.wParam ;
}

/*
	Intiates a synchronized or unsynchronized race containing 5 "cars". Intiates 
	their threads and draws the finish line. Determines winner dependent on 
	the callback message.
*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{

    static HWND		hwndButton[5];
	static WPARAM	buttonNum[5];
	static HANDLE	cars[5];
	PAINTSTRUCT		ps;
	HDC				hdc;
	DWORD			dwRes	= 0;
	int				n		= 0;
	int				y		= 50;
	HPEN			red_pen = CreatePen(PS_SOLID, 5, RGB(155, 55, 80));
	LPCWSTR			names[] = {TEXT("red car"), TEXT("blue car"), TEXT("orange car"), TEXT("black car"), TEXT("green car")};
	HANDLE			quit	= CreateEvent(NULL, FALSE, FALSE, TEXT("race exit"));
	HANDLE			quit2	= CreateEvent(NULL, FALSE, FALSE, TEXT("race exit 2"));
	
	// Checks if program 2/3 have their buttons pressed after intialization.
	dwRes = MsgWaitForMultipleObjects(2, events, false, 0, QS_ALLINPUT);
	switch(dwRes){
		case WAIT_OBJECT_0:
			wndExit[0] = true;
		break;
		
		case WAIT_OBJECT_0 + 1:
			wndExit[1] = true;
		break;
	}
	
	switch (message)
	{

		case WM_CREATE:

				for(int i = 0; i < 5; i++){
					hwndButton[i] = createCar(names[i], n, y, hwnd, lParam);
					buttonNum[i] = (WPARAM)hwndButton[i];
					y += 100;
				}
				
				for(int i = 0; i < 5; i++)
					cars[i] = CreateThread(NULL, 0, startCar, hwndButton[i], CREATE_SUSPENDED, NULL);
				
				UpdateWindow(hwnd);

			return 0;

		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			return 0;
		
		case WM_PAINT:
			hdc = BeginPaint(hwnd, &ps);
			SelectObject(hdc,red_pen);
			MoveToEx(hdc, cxClient/2, 0, NULL);
			LineTo(hdc, cxClient/2, cyClient);
			ReleaseDC(hwnd, hdc);
			return 0;
		
		// Determines winner
		case RACE_CONDITION_TRUE:{
				if(wParam == buttonNum[0]){
					MessageBox(hwnd, TEXT("The red car has won the race!"), TEXT("We have a winner"), MB_OK);
					return 0;
				}
				
				if(wParam == buttonNum[1]){
					MessageBox(hwnd, TEXT("The blue car has won the race!"), TEXT("We have a winner"), MB_OK);
					return 0;
				}
				if(wParam == buttonNum[2]){
					MessageBox(hwnd, TEXT("The orange car has won the race!"), TEXT("We have a winner"), MB_OK);
					return 0;
				}
				if(wParam == buttonNum[3]){
					MessageBox(hwnd, TEXT("The black car has won the race!"), TEXT("We have a winner"), MB_OK);
					return 0;
				}
				if(wParam == buttonNum[4]){
					MessageBox(hwnd, TEXT("The green car has won the race!"), TEXT("We have a winner"), MB_OK);
					return 0;
				}
		return 0;
		}
		
		// Synchronized Race
		case WM_RBUTTONDOWN:
			GLOBAL = TRUE;
			InitializeCriticalSection (&cs);
			
			for(int i = 0; i < 5; i++)
				ResumeThread(cars[i]);
			
			return 0;
		
		// Unsynchronized Race
		case WM_LBUTTONDOWN:
			
			for(int i = 0; i < 5; i++)
				ResumeThread(cars[i]);
			
			return 0;
		
		// Closes program 2 or/and 3 if they signaled "begin"
		case WM_DESTROY:
			
			if(wndExit[0])
				SetEvent(quit);
			
			if(wndExit[1])
				SetEvent(quit2);

			PostQuitMessage (0);
			return 0;
		}
		return DefWindowProc (hwnd, message, wParam, lParam);
}
	
/*
	Starts a "car" thread. "Car" moves towards finish line, once passed, stops.
	Sends a RACE_CONDITION_TRUE message containing the button hwnd (which serves
	as a comparison to determine who sent what message) when it passes the finish 
	line then returns 0.

	A critical section will occur if the GLOBAL is activated.
*/
DWORD WINAPI startCar(LPVOID hwndButton){
	
	RECT carRect;

	GetWindowRect((HWND)hwndButton, &carRect);
	MapWindowPoints(HWND_DESKTOP, GetParent((HWND)hwndButton), (LPPOINT)&carRect, 2);
	
	for(int i = 0; i <= cxClient/2; i += 1){
		MoveWindow((HWND)hwndButton, carRect.left + i, 
			carRect.top, 
			carRect.right - carRect.left, 
			carRect.bottom - carRect.top, 
			TRUE);
		UpdateWindow(mainHWND); 
		Sleep(7);		
	}

	if(GLOBAL)
		EnterCriticalSection(&cs);

	if(!winner)
		SendMessage(mainHWND, RACE_CONDITION_TRUE, (WPARAM)hwndButton, NULL); 
	
	winner = TRUE;

	if(GLOBAL)
		LeaveCriticalSection(&cs);

	return 0;
}

/*
	Creates a new "car". Takes a car name, car id, y car position and the parent
	window hwnd. Retruns a HWND to a button.
*/
HWND createCar(LPCWSTR name, int id, int ypos, HWND hwnd, LPARAM lParam){
	int	width = 80;
	int	height = 30;
	int x = 50;

	return CreateWindow( TEXT("button"),name,
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			x, ypos, width, height, hwnd, 
			(HMENU) id,
			((LPCREATESTRUCT) lParam)->hInstance,
			NULL);
}
