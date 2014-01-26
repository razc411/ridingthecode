/**
	FILE: Drawing.cpp
	DATE: DEC 1 2013
	AUTHOR: Ramzi Chennafi
	FUNCTIONS:
		POINT indexShapes(HWND hWnd, POINT msPrevious, POINT msStart, int HorzPos, int VertPos)
		void setBackground(WPARAM wParam, HWND hWnd, drawStr *dStr)
		POINT indexShapes(HWND hWnd, POINT msPrevious, POINT msStart, int HorzPos, int VertPos)
		void cleanScreen(HDC hdc, HWND hWnd, drawStr *dStr)
		void PaintSelection(HWND hWnd, LPARAM lParam, POINT msStart, POINT msPrevious, int HorzPos, int VertPos)
		void drawLogo(HDC hdc, int clientx, int clienty)
		void miniParty(HRGN hrgn, HWND hWnd, drawStr * dStr)

	Draws shapes and maintains the background color of the drawing screen. Also deals with miniParty.
*/
#include "stdafx.h"
#include "win32Project.h"
#include <random>
#include "functions.h"
/*
	DATE: DEC 1 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION : POINT indexShapes(HWND hWnd, POINT msPrevious, POINT msStart, HPEN* pens, int HorzPos, int VertPos)
	RETURNS : POINT of the current mouse position

	Begins drawing a square or circle from the mouse position depending on the currentShape member of dStr. Also gets the 
	mode, and color of the shape to draw from dStr. Returns the shape end POINT once mouse movement is completed.
	NOTES:
		Stores the shape in the position it was originall drawn in using msPrevious, msStart, HorzPos and VertPos.
		This considers the scroll position.
*/
POINT indexShapes(HWND hWnd, POINT msPrevious, POINT msStart, int HorzPos, int VertPos){
	
	HPEN *pen		= getPens();
	drawStr * dStr	= (drawStr*) GetWindowLong(hWnd, 0);
	RECT rect		= {msStart.x + HorzPos, msStart.y + VertPos, msPrevious.x + HorzPos, msPrevious.y + VertPos};
	//adjusts rect to the current position

	cleanScreen(GetDC(hWnd), hWnd, dStr);

	if(dStr->currentShape == SQUARE){
		dStr->shp.types.push_back(SQUARE);
	}
			
	if(dStr->currentShape == CIRCLE){
		dStr->shp.types.push_back(CIRCLE); 
	}

	dStr->shp.modes.push_back(dStr->mode);
	dStr->shp.shapes.push_back(rect);
	dStr->shp.pens.push_back(dStr->currentPen);
	//shape is now indexed in the struct
	SetWindowLong(hWnd, 0, (LONG) dStr);

	return msPrevious;
} 

/*
	DATE: DEC 1 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION : void setBackground(WPARAM wParam, HWND hWnd, HPEN *pens, drawStr *dStr)

	Sets the background of the main screen to the color specified by the dStr->currentBG member.
*/
void setBackground(WPARAM wParam, HWND hWnd, drawStr *dStr){
	
	COLORREF *	bgColors = getColors();

	HBRUSH		bgBrush	 = CreateSolidBrush(bgColors[dStr->currentBG]);
	RECT		rect;
	//setting brush to current bg  	
	SelectObject((HDC)wParam, bgBrush);
	
	GetClientRect(hWnd, &rect);
	  
	Rectangle((HDC)wParam, rect.left, rect.top, rect.right, rect.bottom);
	
	dStr->lastBG = dStr->currentBG;
	clearShapes(dStr, hWnd); // background change removes all shapes
	SetWindowLong(hWnd, 0, (LONG) dStr);
}
/*
	DATE: DEC 1 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION : void cleanScreen(HDC hdc, HWND hWnd, drawStr *dStr)

	Cleans leftover drawn objects off the screen by repaiting the screen with a square the
	same color of the background.
*/
void cleanScreen(HDC hdc, HWND hWnd, drawStr *dStr){
	
	COLORREF* bgColors = getColors();

	HBRUSH bgBrush;
	RECT rect;
	bgBrush = CreateSolidBrush(bgColors[dStr->currentBG]);
	  	
	SelectObject(hdc, bgBrush);
	//fills the entire screen with the current bg 
	GetClientRect(hWnd, &rect);
	  
	Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom);
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: DWORD WINAPI seizureWindows(LPVOID drStr)
	RETURNS: Error Code on exit, responsible for threading
	
	Randomizes the background color till the end of a timer kills the thread. 
	NOTES: Function is called as a new thread.
*/
DWORD WINAPI seizureWindows(LPVOID drStr){
	drawStr * dStr = (drawStr*) drStr;
	while(1){ // yes, there is a seizure warning in the readme
		int i = (double)rand() / (RAND_MAX + 1) * (RANGE_MAX - RANGE_MIN); // picks random background
		SendMessage(dStr->parentHWND, WM_COMMAND, (DEFAULT_MENU_BG + i), NULL); // sends background changing messages
		Sleep(40);
	}
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void PaintSelection(HWND hWnd, LPARAM lParam, POINT msStart, POINT msPrevious, int HorzPos, int VertPos)
	
	Paints the draw selection box on the screen when drawing a shape. 
*/
void PaintSelection(HWND hWnd, POINT msStart, POINT msPrevious){
	HDC hdc				 = GetDC(hWnd);
	COLORREF	* colors = getColors();
	drawStr		*dStr    = (drawStr*) GetWindowLong(hWnd, 0);
	SCROLLINFO	si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hWnd, SB_VERT, &si); // getting vertical scroll info
	int VertPos = si.nPos;

	GetScrollInfo(hWnd, SB_HORZ, &si); // getting horizontal scroll info
	int HorzPos = si.nPos;

	HBRUSH bg = CreateSolidBrush(colors[dStr->currentBG]); // set brush to currentbg color
	HBRUSH pen = CreateSolidBrush(colors[dStr->currentPen]); // set brush to current pen color
	RECT rect = {msStart.x, msStart.y, msPrevious.x, msPrevious.y}; 
	//paints over the oldbox with the bg color and repaints it each time
	if(dStr->currentShape == SQUARE){
		SelectObject(hdc, bg);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom); // erases old square
		SelectObject(hdc, pen);
		Rectangle(hdc, rect.left, rect.top, rect.right, rect.bottom); // paints new one
	}
	else if(dStr->currentShape == CIRCLE){
		SelectObject(hdc, bg);
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom); // erases old circle
		SelectObject(hdc, pen);
		Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom); // paints new one
	}
	
	ReleaseDC(hWnd, hdc);
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void drawShapes(HDC hdc, int HorzPos, int VertPos)
	RETURNS: NOTHING

	Draws the shapes contained within the programs drawStruct. Called each time
	a WM_PAINT message is recieved. 
*/
void drawShapes(HDC hdc, int HorzPos, int VertPos){
	
	drawStr * dStr = (drawStr*)GetWindowLong(GetActiveWindow(), 0);
	COLORREF * colors = getColors();
	int * modes = getModes();

	//retrieving the current shape vectors
	for(int i = 0; i < dStr->shp.shapes.size(); i++){
		
		//SelectObject(hdc, GetStockObject(DC_BRUSH));
		SetDCBrushColor(hdc, colors[dStr->shp.pens.at(i)]);
		SetROP2(hdc, modes[dStr->shp.modes.at(i)]);

		RECT rt = dStr->shp.shapes.at(i);
		
		switch(dStr->shp.types.at(i)){
			case SQUARE:
				Rectangle(hdc, rt.left-HorzPos, rt.top-VertPos, rt.right-HorzPos, rt.bottom-VertPos);
				//draws shape in the adjusted position
				break;
				
			case CIRCLE:
				Ellipse(hdc,  rt.left-HorzPos, rt.top-VertPos, rt.right-HorzPos, rt.bottom-VertPos);
				break;
		}
	}
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void drawLogo(HDC hdc, int clientx, int clienty)
	RETURNS: NOTHING

	Draws a logo, stays in the middle of the screen regardless of scrolling. Overwritten by
	drawn shapes. Sets the center of the screen to middle of the client rectangle.
	NOTES:
		clientx = right end of the client window
		clienty = bottom end of the client window
*/
void drawLogo(HDC hdc, int clientx, int clienty){
	
	COLORREF * colors = getColors();

	SetViewportOrgEx (hdc, clientx/2, clienty/2, NULL); // sets origin to middle of client screen, for logo centering
	
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, colors[BLUE]);
	Ellipse(hdc, -50, -50, 70, 70);
	
	SelectObject(hdc, GetStockObject(DC_BRUSH));
	SetDCBrushColor(hdc, colors[RED]);
	Rectangle(hdc,-100,-100, 0, 0); 
	
	TextOut(hdc, -80,-19,TEXT("Circles n' Squares"), sizeof("Circles n' Squares"));
	SetViewportOrgEx (hdc, 0, 0, NULL); // returns the screen to the original origin
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void OnPaint(HDC hdc, BOOL drawShape, RECT rect, drawStr * dStr)

	Responsible for drawn shapes and the logo whenever the screen is redrawn.
	Retrieves the current scroll info, cleans the screen, draws the logo, then 
	draws the shapes from drawStr over top.
*/
void OnPaint(HDC hdc, BOOL drawShape, RECT rect, drawStr * dStr){
	SCROLLINFO	si;
	HWND		hWnd = GetActiveWindow();
	
	SetMapMode(hdc, MM_ANISOTROPIC); 
	
	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	GetScrollInfo(hWnd, SB_VERT, &si);
	int VertPos = si.nPos;

	GetScrollInfo(hWnd, SB_HORZ, &si);
	int HorzPos = si.nPos;
			
	if(!drawShape)
		cleanScreen(hdc, hWnd, dStr); // clears screen if a shape isnt being drawn

	drawLogo(hdc, rect.right, rect.bottom); // names explicit enough isnt it?

	if(!drawShape)
		drawShapes(hdc, HorzPos, VertPos);			
}
/*
	DATE: DEC 3 2013
	AUTHOR: Ramzi Chennafi
	FUNCTION: void miniParty(HRGN hrgn, HWND hWnd, drawStr * dStr)

	Starts a mini party in the top left corner of the screen. Drawn using a region and
	fill region. Lasts for 99 color changes then defaults the square to the current background.
	NOTES:
		Done to fufill drawing extents requirements.
*/			
void miniParty(HRGN hrgn, HWND hWnd, drawStr * dStr){
	HDC hdc = GetDC(hWnd);
	COLORREF * colors = getColors();
	int count = 0;
	int i = 0;
	while(count < 100){
		i = (double)rand() / (RAND_MAX + 1) * (RANGE_MAX - RANGE_MIN);
		HBRUSH sb = CreateSolidBrush(colors[i]);
		FillRgn(hdc, hrgn, sb); // fills a rgn with a random color
		count++;
		Sleep(10);
	}
	FillRgn(hdc, hrgn, CreateSolidBrush(colors[dStr->currentBG]));  // sets square back to background color
}