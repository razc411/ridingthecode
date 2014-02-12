/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: drawing.cpp
--
--	PROGRAM : UDP/TCP Transfer in Win32
--
--	FUNCTIONS :
--		void DrawTitleText(HDC hdc)
--		void DrawButtons(HWND hwnd)
--		void DrawDisplay(HWND hwnd)
--		void SetFont(TCHAR* font, HWND hwnd, HWND* hwndButton, int buttons)
--		void Init_Settings(HWND hwnd)
--		void activity(char * buffer, int box)
--
--	DATE: January 20, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	Deals with the graphical interface of the program including the Settings dialog. Draws buttons and text.
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: DrawTitleText
--
--      DATE: January 27, 2014
--      REVISIONS: Adapted for current program.
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
--		Takes the painting HDC as an argument.
----------------------------------------------------------------------------------------------------------------------*/
void DrawTitleText(HDC hdc){

	RECT titleRect = { 500, 500, 1000, 900 };
	LPCSTR title = "UDP/TCP Transfer in Win32";

	SetBkMode(hdc, TRANSPARENT);

	HFONT hFont = CreateFont(38, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 0, 0));

	DrawText(hdc, title, -1, &titleRect, 0);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: DrawButtons
--
--      DATE: January 27, 2014
--      REVISIONS: Adapted for current program.
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
	HWND buttons[6];

	buttons[0] = CreateWindow("BUTTON", "Connect",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		20, 435, 100, 35,
		hwnd, (HMENU)BT_CONNECT, GetModuleHandle(NULL), NULL);

	buttons[1] = CreateWindow("BUTTON", "Disconnect",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		20, 480, 100, 35,
		hwnd, (HMENU)BT_DISCONNECT, GetModuleHandle(NULL), NULL);
	
	buttons[2] = CreateWindow("BUTTON", "Send File",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		256, 435, 100, 35,
		hwnd, (HMENU)BT_SEND, GetModuleHandle(NULL), NULL);
	
	buttons[3] = CreateWindow("BUTTON", "Server Mode",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		137, 480, 100, 35,
		hwnd, (HMENU)ID_CONNECT_SERVERMODE, GetModuleHandle(NULL), NULL);

	buttons[4] = CreateWindow("BUTTON", "Client Mode",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		137, 435, 100, 35,
		hwnd, (HMENU)ID_CONNECT_CLIENTMODE, GetModuleHandle(NULL), NULL);
	
	buttons[5] = CreateWindow("BUTTON", "Mode",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		256, 480, 100, 35,
		hwnd, (HMENU)ID_TRANSFER_MODE, GetModuleHandle(NULL), NULL);
	
	SetFont(_T("Arial"), hwnd, buttons, 6);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: DrawDisplay
--
--      DATE: January 20, 2014
--      REVISIONS: Adapted for current program.
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void DrawDisplay(HWND hwnd)
--
--      RETURNS: void
--
--      NOTES:
--      Draws the 2 display boxes for the program. The first being the EB_STATUSBOX, or the status display and the second
--		being the EB_STATBOX, or the statistics display box.
----------------------------------------------------------------------------------------------------------------------*/
void DrawDisplay(HWND hwnd){
	HWND buttons[2];

	buttons[0] = CreateWindow("EDIT", "",
			ES_LEFT | ES_MULTILINE | WS_VSCROLL | WS_VISIBLE | WS_CHILD | ES_READONLY,
			20, 20, 880, 400,
			hwnd, (HMENU)EB_STATUSBOX, GetModuleHandle(NULL), NULL);

}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: SetFont
--
--      DATE: January 27, 2014
--      REVISIONS: Adapted for current program.
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
	
	for (int i = 0; i < buttons; i++){
		SendMessage(hwndButton[i], WM_SETFONT, (WPARAM)s_hFont, (LPARAM)MAKELONG(TRUE, 0));
	}

	ReleaseDC(hwnd, hdc);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: Init_Settings
--
--      DATE: Febuary 3, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void Init_Settings(HWND hwnd)

--      RETURNS: void
--
--      NOTES:
--      Draws the default connection settings to the Settings dialog box. Takes the parent HWND as an argument in order
--		to retrieve the current settings.
----------------------------------------------------------------------------------------------------------------------*/
void Init_Settings(HWND hwnd){
	HWND hDlgPTCL	= GetDlgItem(hwnd, IDC_PROTSLT);
	HWND hDlgPORT	= GetDlgItem(hwnd, IDC_PORT);
	HWND hDlgIP		= GetDlgItem(hwnd, IDC_IP);
	HWND hDlgSAVE	= GetDlgItem(hwnd, IDC_SDISPLAY);
	HWND hDlgSPORT	= GetDlgItem(hwnd, IDC_SPORT);
	HWND hDlgSPRTCL	= GetDlgItem(hwnd, IDC_SPRTCL);
	HWND hDlgPCKT   = GetDlgItem(hwnd, IDC_PACKETSIZE);
	HWND hDlgTTS	= GetDlgItem(hwnd, IDC_TTS);
	SETTINGS * st = (SETTINGS*) GetClassLongPtr(GetParent(hwnd), 0);
	
	char * packetsizes[] = { "1024", "4096", "20000", "60000"};
	for (int i = 0; i < 4; i++){
		ComboBox_AddString(hDlgPCKT, packetsizes[i]);
	}
	ComboBox_SetCurSel(hDlgPCKT, st->packet_size);
	
	ComboBox_AddString(hDlgSPRTCL, "TCP");
	ComboBox_AddString(hDlgSPRTCL, "UDP");
	ComboBox_SetCurSel(hDlgSPRTCL, st->protocol);
	
	Edit_SetText(hDlgTTS, st->times_to_send);
	Edit_SetText(hDlgSPORT, st->server_port);
	Edit_SetText(hDlgPORT, st->client_port);
	Edit_SetText(hDlgIP, st->client_send_ip);
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: activity
--
--      DATE: January 27, 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void activity(char * buffer, int box)

--      RETURNS: void
--
--      NOTES:
--		Takes a char* containing a message and either EB_STATUSBOX or EB_STATBOX. Will draw the message to either box
--		without erasing its current contents.
----------------------------------------------------------------------------------------------------------------------*/
void activity(char * buffer, int box){
	HWND hDlgStat = GetDlgItem(GetActiveWindow(), EB_STATBOX);
	HWND hDlgStatus = GetDlgItem(GetActiveWindow(), EB_STATUSBOX);
	int ndx = GetWindowTextLength(hDlgStatus);
	
	switch (box){
	case EB_STATUSBOX:
		SetFocus(hDlgStatus);
		SendMessage(hDlgStatus, EM_SETSEL, (WPARAM)ndx, (LPARAM)ndx);
		SendMessage(hDlgStatus, EM_REPLACESEL, 0, (LPARAM)((LPSTR)buffer));
		break;
	case EB_STATBOX:
		Edit_SetText(hDlgStat, buffer);
		break;
	}
}