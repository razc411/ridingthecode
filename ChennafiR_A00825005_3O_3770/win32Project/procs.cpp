/**
	FILE: procs.cpp
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTIONS:
		LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
		INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
		INT_PTR CALLBACK PaintBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)

	Contains the message loops for each respective window of the program. The loops contained are for the
	PaintBox, About Box and the main drawing window. Central to the running of the program.
*/
#include "stdafx.h"
#include "functions.h"
#include "win32Project.h"
/**
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)

	Message loop for the main drawing window of the program. Allows for changing of tools and colors 
	and syncs with the PaintBox.
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	static HANDLE	pThread; // handle to party thread
	static POINT	msPrevious, msStart; // mouse position variables
	static int		VertPos, HorzPos; // scroll adjustment variables
	static HRGN		notice;
	PAINTSTRUCT		ps;
	HDC				hdc;
	RECT			rect; 
	static BOOL		drawShape			= FALSE; // if true, a shape is being drawn
	int				wmId, cxClient	= 0, cyClient = 0;
	HMENU			hMenu				= GetMenu(hWnd);
	drawStr			*dStr				= (drawStr*)GetWindowLong(hWnd, 0);
	GetClientRect(hWnd, &rect);

	switch (message)
	{
		case WM_CREATE:
			notice = CreateRectRgn(0, 0, 200, 200); // creating rgn for miniparty
			break;

		case WM_SIZE:
			cxClient = LOWORD(lParam);
			cyClient = HIWORD(lParam);
			wmSize(hWnd);
			break;

		case WM_HSCROLL:
			HorzPos = hScrollBar(wParam, hWnd);
			break;

		case WM_VSCROLL:
			VertPos = vScrollBar(wParam, hWnd);
			break;

		case WM_TIMER:
			TerminateThread(pThread, 0); // terminates party at the end of the timer
			break;

		case WM_COMMAND:
			wmId    = LOWORD(wParam);
			changeMain(wmId, dStr, dStr->dlgHWND, hWnd);
			topMenu(wmId, dStr, hWnd, notice);
			if(wmId == ID_PARTY){
				SetTimer (hWnd, NULL, 4000, NULL);
				pThread = CreateThread(NULL, 0, seizureWindows, (LPVOID) dStr, 0, 0); //begins the seizures
			}
			break;
		//repaints background with default color
		case WM_ERASEBKGND:
			if(dStr->currentBG != dStr->lastBG){
				setBackground(wParam, hWnd, dStr);
			}
			break;
		//gets mouse position and sets drawshape
		case WM_LBUTTONDOWN:
			if (!drawShape) {
				drawShape = TRUE;
				msPrevious.x = msStart.x = LOWORD(lParam); 
				msPrevious.y = msStart.y = HIWORD(lParam); 
			}
			break;
		//draws a selection box
		case WM_MOUSEMOVE: 
			if(drawShape){
				PaintSelection(hWnd, msStart, msPrevious);
				msPrevious.x = LOWORD(lParam);
				msPrevious.y = HIWORD(lParam);
			}
			break;
		//draws all window elements
		case WM_PAINT:
			hdc = BeginPaint(hWnd, &ps);

			OnPaint(hdc, drawShape, rect, dStr);
			EndPaint(hWnd, &ps);
			break;
		//indexes the shape created by the selection box
		case WM_LBUTTONUP: 
			if (drawShape){
				indexShapes(hWnd, msPrevious, msStart, HorzPos, VertPos);
				drawShape = FALSE;
				InvalidateRect(hWnd, NULL, TRUE);
			}
			break;
		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	
	return 0;
}
/**
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	
	Message loop for the About dialog box of the program. Displays info regarding the program. 
	Exits the dialog on IDOK or IDCANCEL.
*/
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_INITDIALOG:
			return (INT_PTR)TRUE;

		case WM_COMMAND:
	
			if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				EndDialog(hDlg, LOWORD(wParam));
				return (INT_PTR)TRUE;
			}
			break;
	}
	return (INT_PTR)FALSE;
}
/**
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: INT_PTR CALLBACK PaintBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
	
	Message loop for the Paint Toolbox modeless dialog of the program. Provides easy access to change
	drawing options for the main window. Includes pen mode (XOR/Normal/Inverse/NOTXOR), BG and Pen colors,
	as well as an erase screen button. Syncs with the main window menu.
*/
INT_PTR CALLBACK PaintBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId;
	HWND hwndParent = GetParent(hDlg);
	HMENU hMenuParent = GetMenu(hwndParent);
	drawStr *dStr = (drawStr*) GetWindowLong(hwndParent, 0);
	dStr->dlgHWND = hDlg;
	SetWindowLong(hwndParent, 0, (LONG) dStr);

	switch (message)
	{
		case WM_INITDIALOG: // sets intial menu options
			checkPen(BLACK, hwndParent);
			checkBG(WHITE, hwndParent);
			checkShape(SQUARE, hwndParent);
			checkMode(NORMAL, hwndParent);
			return (INT_PTR)TRUE;
		
		case WM_COMMAND: 
			wmId    = LOWORD(wParam);
			// changes paint box option buttons according to dStr
			changeDbox(wmId, dStr, hDlg, hwndParent); 
			switch(wmId){
				case IDC_ERASE: // erases shapes
					clearShapes(dStr, hwndParent);
					cleanScreen(GetDC(hwndParent), hwndParent, dStr);
					break;
			}
			break;
	}
	return (INT_PTR)FALSE;
}
