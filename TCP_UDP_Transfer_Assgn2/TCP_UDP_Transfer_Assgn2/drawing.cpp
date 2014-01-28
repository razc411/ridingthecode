/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: drawing.cpp
--
--	PROGRAM : UDP/TCP Transfer in Win32
--
--	FUNCTIONS :
--		int main()
--		void process_input(int rawp[2], int transp[2], pid_t * children)
--		void translate_input(int frmtp[2], int transp[2])
--		void print_output(int rawp[2], int frmtp[2])
--		void fatal_error(char * error)
--
--	DATE: January 20, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	A program which disables terminal text processing and handles it instead.Several methods of character input have
--	changed, as well as methods of terminating the program.Prints out one line of raw input, then upon typing "E",
--	prints out a formatted line of text.All 'a' characters are changed to 'z', all 'z' characters are changed to 'a',
--	and control letters such as X, E, K and T are not printed in the formatted text.
--
--
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: DrawTitleText
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void DrawTitleText(HDC hdc)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function that draws various GUI text labels in the program. Called by WM_PAINT each screen refresh.
----------------------------------------------------------------------------------------------------------------------*/
void DrawTitleText(HDC hdc){

	RECT titleRects[] = { { 500, 650, 1000, 900 } };
	LPCSTR titles[] = { "UDP/TCP Transfer in Win32"};
	HFONT hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);

	for (int i = 1; i < (sizeof(titleRects) / sizeof(titleRects[0])); i++){
		DrawText(hdc, titles[i], -1, &titleRects[i], 0);
	}

	hFont = CreateFont(38, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 0, 0));

	DrawText(hdc, titles[0], -1, &titleRects[0], 0);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: DrawButtons
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void DrawButtons(HWND hwnd)
--
--      RETURNS: void
--
--      NOTES:
--      Generic function to draw buttons. Font is set at the end for all the buttons in the function.
----------------------------------------------------------------------------------------------------------------------*/
void DrawButtons(HWND hwnd){
	HWND buttons[3];

	buttons[0] = CreateWindow("BUTTON", "Connect",
							WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
							60, 500, 100, 35,
							hwnd, (HMENU)BT_CONNECT, GetModuleHandle(NULL), NULL);

	buttons[1] = CreateWindow("BUTTON", "Disconnect",
							WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
							180, 500, 100, 35,
							hwnd, (HMENU)BT_DISCONNECT, GetModuleHandle(NULL), NULL);
	
	buttons[1] = CreateWindow("BUTTON", "Send File",
							WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
							300, 500, 100, 35,
							hwnd, (HMENU)BT_SEND, GetModuleHandle(NULL), NULL);
	
	SetFont(_T("Arial"), hwnd, buttons, 3);
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
void DrawDisplay(HWND hwnd){
	HWND buttons[2];

	buttons[0] = CreateWindow("EDIT", "",
							ES_LEFT| ES_MULTILINE | ES_AUTOVSCROLL | WS_VISIBLE | WS_CHILD | ES_READONLY,
							20, 20, 600, 400,
							hwnd, (HMENU)EB_STATUSBOX, GetModuleHandle(NULL), NULL);
	
	buttons[1] = CreateWindow("EDIT", "",
							ES_LEFT | ES_MULTILINE | ES_AUTOVSCROLL | WS_VISIBLE | WS_CHILD | ES_READONLY,
							630, 20, 280, 400,
							hwnd, (HMENU)EB_STATBOX, GetModuleHandle(NULL), NULL);

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
void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons){
	HDC hdc = GetDC(hwnd);
	
	static HFONT s_hFont = NULL;
	const long nFontSize = 10;

	LOGFONT logFont = { 0 };
	logFont.lfHeight = -MulDiv(nFontSize, GetDeviceCaps(hdc, LOGPIXELSY), 72);
	logFont.lfWeight = FW_BOLD;
	_tcscpy_s(logFont.lfFaceName, font);

	s_hFont = CreateFontIndirect(&logFont);
	
	for (size_t i = 0; i < buttons; i++){
		SendMessage(hwndButton[i], WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
	}

	ReleaseDC(hwnd, hdc);

}