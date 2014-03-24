/*
	FILE: Intialize.cpp
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTIONS: 
		int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
		ATOM MyRegisterClass(HINSTANCE hInstance, TCHAR* szWindowClass)
		HWND InitInstance(HINSTANCE hInstance, int nCmdShow, TCHAR* szWindowClass, TCHAR* szTitle)
	
	Starts the program, registers and intiates windows and begins a message loop.
*/
#include "stdafx.h"
#include "win32Project.h"
#include "functions.h"
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)

	Starts the program, intiates windows and the message loop.
*/
int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	TCHAR	szTitle[MAX_LOADSTRING] = TEXT("Circles N' Squares");					
	TCHAR	szWindowClass[MAX_LOADSTRING] = TEXT("WIN32PROJECTCNS");			
	HWND	hlgModeless;
	MSG		msg;
	
	MyRegisterClass(hInstance, szWindowClass);
	//Registering the Modeless Dialog Box
	hlgModeless = InitInstance (hInstance, nCmdShow, szWindowClass, szTitle);

	while(GetMessage(&msg, NULL, 0, 0)){
		if(hlgModeless == 0 || !IsDialogMessage(hlgModeless, &msg)){ // sends messages to the modeless box if they're for it
				TranslateMessage(&msg);
				DispatchMessage(&msg);
		}
	}
	
	return (int) msg.wParam;
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: ATOM MyRegisterClass(HINSTANCE hInstance, TCHAR* szWindowClass)

	Registers a window class.
*/
ATOM MyRegisterClass(HINSTANCE hInstance, TCHAR* szWindowClass)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= sizeof(drawStr*);
	wcex.hInstance		= hInstance;
	wcex.hIcon			= NULL;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH) GetStockObject (WHITE_BRUSH);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= NULL;

	return RegisterClassEx(&wcex);
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: HWND InitInstance(HINSTANCE hInstance, int nCmdShow, TCHAR* szWindowClass, TCHAR* szTitle)

	Intiates the registered windows of the program. Includes PaintBox and the main drawing window. 
	Sets the drawStruct Class variable of the main window.
*/
HWND InitInstance(HINSTANCE hInstance, int nCmdShow, TCHAR* szWindowClass, TCHAR* szTitle)
{
   HWND				hWnd;
   HWND				dlgModeless;
   static drawStr	intial;
   //the changing size of the struct made mallocing a very difficult thing to implement, this seems
   //to work without the data going arwy
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);
   
   if (!hWnd)
      return NULL;
  
   intial.currentBG		= WHITE;
   intial.currentPen	= BLACK;
   intial.currentShape	= SQUARE;
   intial.mode			= NORMAL;
   intial.parentHWND	= hWnd;
   
   SetWindowLong(hWnd, 0, (LONG) &intial);
   //Calling setclasslong after setting the modeless box seems to cause the program to break
   intial.dlgHWND		= dlgModeless = CreateDialog(hInstance, MAKEINTRESOURCE(IDD_PAINTBOX), hWnd, PaintBox);
   
   SetWindowLong(hWnd, 0, (LONG) &intial);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
   ShowWindow(dlgModeless, nCmdShow);
   UpdateWindow(dlgModeless);

   return dlgModeless;
}
