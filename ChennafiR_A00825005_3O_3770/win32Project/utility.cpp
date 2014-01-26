/*
	FILE: Utility.cpp
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTIONS: 
		void clearShapes(drawStr * dStr, HWND hwnd)
		int vScrollBar(WPARAM wParam, int VertPos, SCROLLINFO *si, HWND hWnd)
		void wmSize(HWND hWnd)
		int hScrollBar(WPARAM wParam, int HorzPos, SCROLLINFO * si, HWND hWnd)
		void setClipboard()
		void getClipboard()
		HPEN* getPens()
		COLORREF* getColors()
		int* getModes()
	
	Utility class that stores general use functions for the program. Also manages the scrollbars
	and clipboard.
*/
#include "stdafx.h"
#include "functions.h"
#include "win32Project.h"
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void clearShapes(drawStr * dStr, HWND hwnd)
	
	Clears all shapes from the drawStr struct.
	NOTES:
		Calls .clear() on the vectors containing information pertaining to
		shape creation.
*/
void clearShapes(drawStr * dStr, HWND hwnd){
	
	dStr->shp.modes.clear();
	dStr->shp.types.clear();
	dStr->shp.shapes.clear();
	dStr->shp.pens.clear();
	SetWindowLong(hwnd, 0, (LONG) dStr);
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: int vScrollBar(WPARAM wParam, int VertPos, SCROLLINFO *si, HWND hWnd, RECT rect)
	RETURNS: int - the current Vertical Position
	
	Handles vertical scrollbar messages.
*/
int vScrollBar(WPARAM wParam, HWND hWnd){
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	
	GetScrollInfo(hWnd, SB_VERT, &si);
	int VertPos = si.nPos;

	switch(LOWORD(wParam)){
		case SB_TOP:
			si.nPos = si.nMin;
			break;

		case SB_BOTTOM:
			si.nPos = si.nMax;
			break;

		case SB_LINEUP:
			si.nPos--;
			break;

		case SB_LINEDOWN:
			si.nPos++;
			break;

		case SB_PAGEUP:
			si.nPos -= si.nPage;
			break;

		case SB_PAGEDOWN:
			si.nPos += si.nPage;
			break;

		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);
	GetScrollInfo(hWnd, SB_VERT, &si);

	if (si.nPos != VertPos){
		InvalidateRect(hWnd, NULL, TRUE);
	}
	
	return VertPos;
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void wmSize(HWND hWnd)
	
	Sets the scrollbar settings for the main window.
*/
void wmSize(HWND hWnd){
	
	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0; // sets min window size
	si.nMax = 1000; // sets max window size
	si.nPage = 35; // sets paging size
	si.nPos = 0;
	SetScrollInfo(hWnd, SB_VERT, &si, TRUE);

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;
	si.nMin = 0;
	si.nMax = 1000;
	si.nPos = 0;
	si.nPage = 35;
	SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: int hScrollBar(WPARAM wParam, int VertPos, SCROLLINFO *si, HWND hWnd, RECT rect)
	RETURNS: int - the current horizontal position
	
	Handles horizontal scrollbar messages.
*/
int hScrollBar(WPARAM wParam, HWND hWnd){
	SCROLLINFO si;
	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	GetScrollInfo(hWnd, SB_HORZ, &si);
	int HorzPos = si.nPos;

	switch (LOWORD(wParam)) // deals with changes in scrolling
	{
		case SB_LINELEFT:
			si.nPos -=1;
			break;

		case SB_LINERIGHT:
			si.nPos += 1;
			break;

		case SB_PAGELEFT:
			si.nPos -= si.nPage;
			break;

		case SB_PAGERIGHT:
			si.nPos += si.nPage;
			break;

		case SB_THUMBTRACK:
			si.nPos = si.nTrackPos;
			break;

		default:
			break;
	}

	si.fMask = SIF_POS;
	SetScrollInfo(hWnd, SB_HORZ, &si, TRUE);
	GetScrollInfo(hWnd, SB_HORZ, &si);

	if (si.nPos != HorzPos){
		InvalidateRect(hWnd, NULL, TRUE);
	}
	
	return HorzPos;
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void setClipboard()
	
	Sends the last drawn shape of the program from the drawStruct to the clipboard
	under the ID "Shape"
	NOTES:
		Previous was all the shapes, but ran into issues with pointers and ran out
		of time. So now it only sends one. 
		Shape is drawn exactly how it was in one program to another. Draws over top 
		of existing shapes.
*/
void setClipboard(){
	UINT		DataId			= RegisterClipboardFormat(TEXT("Shape")); // copy/paste ID
	drawStr		*dStr			= (drawStr*)GetWindowLong(GetActiveWindow(), 0);
	
	GLOBALHANDLE hShape 		= GlobalAlloc(GHND, (sizeof(ShapeCopy)));
	ShapeCopy	*pShape 	    = (ShapeCopy*) GlobalLock(hShape);

	int last = dStr->shp.shapes.size() - 1; // the last item in the shapes vector

	pShape->mode = dStr->shp.modes.at(last);
	pShape->type = dStr->shp.types.at(last);
	pShape->shape = dStr->shp.shapes.at(last);
	pShape->pen = dStr->shp.pens.at(last);

	GlobalUnlock(hShape);
	OpenClipboard(GetActiveWindow());
	EmptyClipboard();
	SetClipboardData(DataId, hShape);
	CloseClipboard();
	MessageBox(GetActiveWindow(), TEXT("Shape Copied"), TEXT("Copy N Paste"), MB_ICONEXCLAMATION);
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void getClipboard()
	
	Gets the contents of the clipboard, checking for the "Shape" ID. 
	Should grab a single shape and draw it ontop of existing shapes.
	NOTE: 
		Small bug, sometimes it doesn't redraw after pasting. Make sure 
		to scroll or draw if it doesnt appear.
*/
void getClipboard(){
	HWND		hMain		 = GetActiveWindow();
	drawStr		*dStr		 = (drawStr*)GetWindowLong(hMain, 0);
	UINT		DataId		 = RegisterClipboardFormat(TEXT("Shape"));

	OpenClipboard(hMain);	
	GLOBALHANDLE hShape 		 = GetClipboardData(DataId); // gets Shapes ID data
	ShapeCopy	*pShape 		 = (ShapeCopy*) GlobalLock(hShape);
	
	dStr->shp.modes.push_back(pShape->mode);
	dStr->shp.types.push_back(pShape->type);
	dStr->shp.pens.push_back(pShape->pen);
	dStr->shp.shapes.push_back(pShape->shape);
	//setting current drawStruct from the main class to the revised one
	SetWindowLong(hMain, 0, (LONG) dStr);
	InvalidateRect(GetActiveWindow(), NULL, TRUE);
	GlobalUnlock(hShape);
	CloseClipboard();
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: HPEN* getPens
	RETURNS: An array of HPENs
	
	Returns an array of HPENs, for easy use.Asking for the RED, GREEN, BLUE, 
	WHITE and BLACK members from the array will give you the corresponding pens.

*/
HPEN* getPens(){
	static HPEN pens[] = {CreatePen(PS_SOLID, 2, RGB(255, 000, 000)), 
						  CreatePen(PS_SOLID, 2, RGB(000, 000, 255)), 
						  CreatePen(PS_SOLID, 2, RGB(000, 255, 000)),
						  CreatePen(PS_SOLID, 2, RGB(000, 000, 000)),
						  CreatePen(PS_SOLID, 2, RGB(255, 255, 255))};
	return pens;
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: HPEN* getColors
	RETURNS: An array of COLORREFs
	
	Returns an array of COLORREFs, for easy use.Asking for the RED, GREEN, BLUE, 
	WHITE and BLACK members from the array will give you the corresponding COLORREF.

*/
COLORREF* getColors(){
	static COLORREF bgColors[] = {RGB(255,000,000), RGB(000,000,255), 
								  RGB(000,255,000), RGB(000,000,000), RGB(255,255,255)};
	return bgColors;
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: int* getModes()
	RETURNS: An array of int modes
	
	Returns an array of pen modes. Calling from the array index NORMAL XOR NOTXOR 
	or INVERSE will give you the corresponding value to those modes.

*/
int* getModes(){
	static int mode[] = {R2_COPYPEN, R2_XORPEN, R2_NOTCOPYPEN, R2_NOTXORPEN};

	return mode;
}