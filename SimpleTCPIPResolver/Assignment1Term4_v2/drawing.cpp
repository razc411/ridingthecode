//-------------------------------------------------------------------------------------------------------
//	PROJECT: TCP/IP Resolver
//	FILE: drawing.cpp
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	FUNCTIONS: 
//			HWND Create_Ok_Button(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name)
//			HWND Create_Edit_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name)
//			HWND Create_CB_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name)
//			HWND Create_Clear_Button(HWND hwnd, HINSTANCE hInst)
//			HWND Create_Output_Box(HWND hwndParent, HINSTANCE hInst)
//			void DrawTitleText(HDC hdc)
//	DESCRIPTION:
//			Generic functions for creating GUI aspects, includes a function to create text targeted at
//			this specific program.
//	NOTES:
//			name is a specified resource handle. Requires WS2_32.lib for winsock. Character set must be 
//			"not set", TCHAR -> char* conversion problems will arise.
//-------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "Assignment1Term4_v2.h"
//-------------------------------------------------------------------------------------------------------
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: HWND Create_Ok_Button(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name)
//	RETURNS: HWND to the ok button created
//	DESCRIPTION:
//			Creates an OK button at the specified position with a length of 80 and height of 25 with the
//			the specified resource handle.
//-------------------------------------------------------------------------------------------------------
HWND Create_Ok_Button(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name){

	HWND hwndButton = CreateWindow("BUTTON", "OK",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		xcoord, ycoord, 80, 25,
		hwndParent, (HMENU)name, hInst, NULL);
	
	return hwndButton;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: HWND Create_Edit_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name)
//	RETURNS: HWND to the edit box created
//	DESCRIPTION:
//			Creates an edit box with a length of 150 and height of 24 at the specified position with
//			the specified resource handle.
//-------------------------------------------------------------------------------------------------------
HWND Create_Edit_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name){
	HWND hwndCtrl = CreateWindow("EDIT", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | ES_MULTILINE | ES_AUTOHSCROLL,
		xcoord, ycoord, 150, 24,
		hwndParent, (HMENU)name, hInst, NULL);

	return hwndCtrl;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: HWND Create_CB_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name)
//	RETURNS: HWND to the combo box created
//	DESCRIPTION:
//			Creates a dropdownlist combo box with the specified resource handle at the specified location.
//-------------------------------------------------------------------------------------------------------
HWND Create_CB_Ctrl(HWND hwndParent, int xcoord, int ycoord, HINSTANCE hInst, int name){
	HWND hwndCtrl = CreateWindow("COMBOBOX", "",
		WS_VISIBLE | WS_CHILD | WS_BORDER | CBS_DROPDOWNLIST,
		xcoord, ycoord, 80, 100,
		hwndParent, (HMENU)name, hInst, NULL);

	return hwndCtrl;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: HWND Create_Output_Box(HWND hwndParent, HINSTANCE hInst)
//	RETURNS: HWND to the output box created.
//	DESCRIPTION:
//			Creates the output Listbox for resolver results. The resource handle created is ID_EDIT_BOX.
//-------------------------------------------------------------------------------------------------------
HWND Create_Output_Box(HWND hwndParent, HINSTANCE hInst){
	HWND hwndOutput = CreateWindow("LISTBOX", "",
		WS_VISIBLE | WS_CHILD | LBS_HASSTRINGS | WS_VSCROLL,
		530, 20, 350, 650,
		hwndParent, (HMENU)ID_EDIT_BOX, hInst, NULL);

	return hwndOutput;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: HWND Create_Clear_Button(HWND hwnd, HINSTANCE hInst)
//	RETURNS: HWND to the clear button created.
//	DESCRIPTION:
//			Creates the clear button. Clears the Listbox when pressed.
//-------------------------------------------------------------------------------------------------------
HWND Create_Clear_Button(HWND hwnd, HINSTANCE hInst){
	HWND hwndButton = CreateWindow("BUTTON", "CLEAR",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		530, 664, 351, 25,
		hwnd, (HMENU)ID_CLEAR, hInst, NULL);
	return hwndButton;
}

//-------------------------------------------------------------------------------------------------------
//	DATE: January 13 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: void DrawTitleText(HDC hdc)
//	RETURNS: Nothing
//	DESCRIPTION:
//			Draws GUI title text in Arial.
//-------------------------------------------------------------------------------------------------------
void DrawTitleText(HDC hdc){
	
	RECT titleRects[] = { { 180, 120, 300, 220 }, { 180, 260, 300, 350 }, { 180, 400, 300, 440 }, { 180, 540, 300, 580 }, { 120, 50, 450, 450 } };
	LPCSTR titles[] = { "TCP/IP Resolver", "IP -> Host Name", "Host Name -> IP", "Port -> Service", "Service -> Port" };
	HFONT hFont = CreateFont(20, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 255, 255));
	SetBkMode(hdc, TRANSPARENT);
	
	for (int i = 0; i < 5; i++){
		DrawText(hdc, titles[i], -1, &titleRects[i-1], 0);
	}

	hFont = CreateFont(38, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE, ANSI_CHARSET, 
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, "Arial");

	SelectObject(hdc, hFont);
	SetTextColor(hdc, RGB(255, 0, 0));

	DrawText(hdc, titles[0], -1, &titleRects[4], 0);
}