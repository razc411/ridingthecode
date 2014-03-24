/*
	FILE: Menu.cpp
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTIONS: 
		bool changeMain(int wmId, drawStr * dStr, HWND hDlg, HWND hMain)
		void checkPen(int selection, HWND hMain)
		void checkMode(int selection, HWND hMain)
		void checkShape(int selection, HWND hMain)
		void checkBG(int selection, HWND hMain)
		void changeDbox(int wmId, drawStr * dStr, HWND hDlg, HWND hMain)
		void topMenu(int wmId, drawStr * dStr, HWND hWnd, HRGN notice)

	Changes tool options and keeps menus synced in the PaintBox and main window.
*/
#include "stdafx.h"
#include "functions.h"
#include "win32Project.h"
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void checkPen(int selection, HWND hMain)

	Keeps the pen selection buttons on both the menu and paintbox synced. Also sets current value.
	Interacts with both the paint box and the main menu.
*/
void checkPen(int selection, HWND hMain){

	drawStr * current = (drawStr*)GetWindowLong(hMain, 0);
	HMENU hMenu = GetMenu(current->parentHWND);

	CheckDlgButton(current->dlgHWND, DEFAULT_COLOR_PEN + current->currentPen, BST_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_PEN + current->currentPen, MF_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_PEN + selection, MF_CHECKED);
	CheckDlgButton(current->dlgHWND, DEFAULT_COLOR_PEN + selection, BST_CHECKED);

	//changing current settings
	current->currentPen = selection;
	SetWindowLong(hMain, 0, (LONG) current);
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void checkBG(int selection, HWND hMain)

	Keeps the shape selection buttons on both the menu and paintbox synced. Also sets current value.
	Interacts with both the paint box and the main menu.
*/
void checkBG(int selection, HWND hMain){
	drawStr * current = (drawStr*)GetWindowLong(hMain, 0);
	HMENU hMenu = GetMenu(current->parentHWND);

	CheckDlgButton(current->dlgHWND, DEFAULT_COLOR_BG + current->currentBG, BST_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_BG + current->currentBG, MF_UNCHECKED);
	CheckDlgButton(current->dlgHWND, DEFAULT_COLOR_BG + selection, BST_CHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_BG + selection, MF_CHECKED);

	//changing current settings
	current->currentBG = selection;
	SetWindowLong(hMain, 0, (LONG) current);
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void checkShape(int selection, HWND hMain)

	Keeps the shape selection buttons on both the menu and paintbox synced. Also sets current value.
	Interacts with both the paint box and the main menu.
*/
void checkShape(int selection, HWND hMain){
	HMENU hMenu = GetMenu(hMain);
	drawStr * current = (drawStr*)GetWindowLong(hMain, 0);

	CheckDlgButton(current->dlgHWND, DEFAULT_SHAPE + current->currentShape, BST_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_SHAPE + current->currentShape, MF_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_SHAPE + selection, MF_CHECKED);
	CheckDlgButton(current->dlgHWND, DEFAULT_SHAPE + selection, BST_CHECKED);
	
	//changing current settings
	current->currentShape = selection;
	SetWindowLong(hMain, 0, (LONG) current);
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void checkMode(int selection, HWND hMain)

	Keeps the mode selection buttons on both the menu and paintbox synced. Also sets current value.
	Interacts with both the paint box and the main menu.
*/
void checkMode(int selection, HWND hMain){
	HMENU hMenu = GetMenu(hMain);
	drawStr * current = (drawStr*)GetWindowLong(hMain, 0);

	CheckDlgButton(current->dlgHWND, DEFAULT_MODE + current->mode, BST_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_MODE + current->mode, MF_UNCHECKED);
	CheckMenuItem(hMenu, DEFAULT_MENU_MODE + selection, MF_CHECKED);
	CheckDlgButton(current->dlgHWND, DEFAULT_MODE + selection, BST_CHECKED);

	//changing current settings
	current->mode = selection;
	SetWindowLong(hMain, 0, (LONG) current);
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void changeDbox(int wmId, drawStr * dStr, HWND hDlg, HWND hMain)

	Changes the tool options whenever an option is changed in the PaintBox 
	window. Also erases the screen when erase is hit.
*/
void changeDbox(int wmId, drawStr * dStr, HWND hDlg, HWND hMain){
	
	int selection = 0;

	switch(wmId){
		
		case IDC_MODE_NORMAL: case IDC_MODE_XOR: case IDC_MODE_INVERSE: case IDC_MODE_NOTXOR:
			selection = wmId - DEFAULT_MODE;
			checkMode(selection, hMain);
			break;
		
		case IDC_PEN_RED: case IDC_PEN_BLUE: case IDC_PEN_GREEN: case IDC_PEN_BLACK: case IDC_PEN_WHITE:
			selection = wmId - DEFAULT_COLOR_PEN;
			checkPen(selection, hMain);
			break;
		
		case IDC_BG_RED: case IDC_BG_BLUE: case IDC_BG_GREEN: case IDC_BG_BLACK: case IDC_BG_WHITE:
			selection = wmId - DEFAULT_COLOR_BG;
			checkBG(selection, hMain);
			InvalidateRect(hMain,NULL, TRUE);
			break;
		
		case IDC_SHAPE_SQUARE: case IDC_SHAPE_CIRCLE:
			selection = wmId - DEFAULT_SHAPE;
			checkShape(selection, hMain);
			break;
	}	
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: bool changeMain(int wmId, drawStr * dStr, HWND hDlg, HWND hMain)
	RETURNS: true if a case is triggered, false if not.

	Changes the tool options whenever an option is changed in the main window. 
*/
bool changeMain(int wmId, drawStr * dStr, HWND hDlg, HWND hMain){
	
	int selection = 0;

	switch(wmId){
		
		case ID_MODE_NORMAL: case ID_MODE_XOR: case ID_MODE_INVERSE: case ID_MODE_NOTXOR:
			selection = wmId - DEFAULT_MENU_MODE;
			checkMode(selection, hMain);
			return true;

		case ID_PEN_RED: case ID_PEN_BLUE: case ID_PEN_GREEN: case ID_PEN_BLACK: case ID_PEN_WHITE:
			selection = wmId - DEFAULT_MENU_PEN;
			checkPen(selection, hMain);
			return true;
		
		case ID_BACKGROUND_RED: case ID_BACKGROUND_BLUE: case ID_BACKGROUND_GREEN: case ID_BACKGROUND_BLACK: case ID_BACKGROUND_WHITE:
			selection = wmId - DEFAULT_MENU_BG;
			checkBG(selection, hMain);
			InvalidateRect(hMain,NULL, TRUE);
			return true;
		
		case ID_SHAPE_SQUARE: case ID_SHAPE_CIRCLE:
			selection = wmId - DEFAULT_MENU_SHAPE;
			checkShape(selection, hMain);
			return true;
		
		case IDM_ABOUT:
			DialogBox(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_ABOUTBOX), hMain, About);
			return true;

		case IDM_EXIT:
			DestroyWindow(hMain);
			return true;
	}

	return false;
}
/*
	DATE: December 2 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void topMenu(int wmId, drawStr * dStr, HWND hWnd, HRGN notice)

	Serves to shorten WNDPROC, takes the WM_COMMAND event and responds accordingly.
	Contains menu commands pertaining to MINI_PARTY, copy and paste.
	NOTES: 
		Takes in a HRGN to draw the miniparty. Done as something to fill the drawing
		extents requirement.
*/
void topMenu(int wmId, drawStr * dStr, HWND hWnd, HRGN notice){
	switch(wmId){
			case ID_MINI_PARTY:
				miniParty(notice, hWnd, dStr);
				break;
			case ID_COPY:
				setClipboard();
				break;
			case ID_PASTE:
				getClipboard();
				break;
		}
}