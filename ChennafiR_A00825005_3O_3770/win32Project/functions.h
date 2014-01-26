#pragma once
//Contains all headers for the functions of this program
//Intialize.cpp		-- intializes main window, class pointers and dialogs, contains message loop
int APIENTRY		_tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR    lpCmdLine, int nCmdShow);
ATOM				MyRegisterClass(HINSTANCE hInstance, TCHAR* szWindowClass);
HWND				InitInstance(HINSTANCE hInstance, int nCmdShow, TCHAR* szWindowClass, TCHAR* szTitle);
// Procs.cpp		-- contains all the in use procs for the 3 windows, PaintBox, About and WIN32PROJECTMAIN
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	PaintBox(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
// Menu.cpp			--functions that affect the menu and paintbox
void				changeDbox(int wmId, drawStr * dStr, HWND hDlg, HWND hMain);
bool				changeMain(int wmId, drawStr * dStr, HWND hDlg, HWND hMain);
void				checkPen(int selection, HWND hMain);
void				checkBG(int selection, HWND hMain);
void				checkShape(int selection, HWND hMain);
void				checkMode(int selection, HWND hMain);
void    			topMenu(int wmId, drawStr * dStr, HWND hWnd, HRGN notice);
// Drawing.cpp		-- functions that draw the main window
POINT				indexShapes(HWND hWnd, POINT msPrevious, POINT msStart, int HorzPos, int VertPos);
void				setBackground(WPARAM wParam, HWND hWnd, drawStr *dStr);
DWORD WINAPI		seizureWindows(LPVOID drStr);
void				PaintSelection(HWND hWnd, POINT msStart, POINT msPrevious);
void				drawShapes(HDC hdc, int HorzPos, int VertPos);
void				cleanScreen(HDC hdc, HWND hWnd, drawStr *dStr);
void				drawLogo(HDC hdc, int clientx, int clienty);
void				miniParty(HRGN hrgn, HWND hWnd, drawStr * dStr);
void				OnPaint(HDC hdc, BOOL drawShape, RECT rect, drawStr * dStr);
// Utility.cpp		-- general use functions
void				setClipboard();
void				getClipboard();
void				clearShapes(drawStr *dStr, HWND hwnd);
HPEN*				getPens();
COLORREF*			getColors();
int*				getModes();
int	    			vScrollBar(WPARAM wParam, HWND hWnd);
void				wmSize(HWND hWnd);
int 				hScrollBar(WPARAM wParam, HWND hWnd);