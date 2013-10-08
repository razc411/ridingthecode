/*------------------------------------------------------------
Synchro Assignment
------------------------------------------------------------*/
#include "resource.h"

HWND mainHWND;
static int cxClient = 800, cyClient = 600;

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
					PSTR szCmdLine, int iCmdShow)
{
	static TCHAR szAppName[] = TEXT ("Extreme Racing") ;
	HWND         hwnd ;
	MSG          msg ;
	WNDCLASS     wndclass ;

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

	hwnd = CreateWindow (szAppName,                  // window class name
		TEXT ("Extreme Racing"), // window caption
		WS_OVERLAPPEDWINDOW,        // window style
		CW_USEDEFAULT,              // initial x position
		CW_USEDEFAULT,              // initial y position
		800,              // initial x size
		600,              // initial y size
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

LRESULT CALLBACK WndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND	  hwndButton[5];
	static WPARAM buttonNum[5];
	static HANDLE cars[5];
	int			  n = 0;
	LPCWSTR		  names[] = {TEXT("red car"), TEXT("blue car"), TEXT("orange car"), TEXT("black car"), TEXT("green car")};
	
	//struct carData **cData = (struct carData **) malloc(sizeof(carData));
	
	int y = 50;

	switch (message)
	{
		case WM_CREATE:
				
				DWORD threadID;

				for(int i = 0; i < 5; i++){
					hwndButton[i] = createCar(names[i], n, y, hwnd, lParam);
					buttonNum[i] = (WPARAM)hwndButton[i];
					y += 100;
				}
				
				for(int i = 0; i < 5; i++){
					cars[i] = CreateThread(NULL, 0, startCar, hwndButton[i], CREATE_SUSPENDED, &threadID);
				}

				UpdateWindow(hwnd);
			return 0;

		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			return 0;
		
		case RACE_CONDITION_TRUE:{
				if(wParam == buttonNum[0])
					MessageBox(hwnd, TEXT("Red has Won"), TEXT("We have a winner"), MB_OK);
				return 0;
				
				if(wParam == buttonNum[1])
					MessageBox(hwnd, TEXT("Blue has Won"), TEXT("We have a winner"), MB_OK);
				return 0;
				
				if(wParam == buttonNum[2])
					MessageBox(hwnd, TEXT("Orange has Won"), TEXT("We have a winner"), MB_OK);
				return 0;
				
				if(wParam == buttonNum[3])
					MessageBox(hwnd, TEXT("Black has Won"), TEXT("We have a winner"), MB_OK);
				return 0;
				
				if(wParam == buttonNum[4])
					MessageBox(hwnd, TEXT("Green has Won"), TEXT("We have a winner"), MB_OK);
		}
		return 0;
			

		case WM_LBUTTONDOWN:
			for(int i = 0; i < 5; i++){
				ResumeThread(cars[i]);
			}
		
			//unsynchro race

			return 0;

		case WM_RBUTTONDOWN:
			//synchro race

			return 0;

		case WM_PAINT:

			return 0 ;

		case WM_DESTROY:
			PostQuitMessage (0) ;
			return 0 ;
		}
		return DefWindowProc (hwnd, message, wParam, lParam);
}
	

DWORD WINAPI startCar(LPVOID hwndButton){
	
	RECT carRect;
	//struct carData *raceData;
	//raceData = (struct carData *) raceData;
	
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
	
	SendMessage(mainHWND, RACE_CONDITION_TRUE, (WPARAM)hwndButton, NULL); 

	return 0;
}

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