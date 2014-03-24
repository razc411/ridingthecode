// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently

#pragma once

#include <windows.h>
// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <vector>
//STRUCTS
//Contains the structs with the shapes.
//Uses C++ vectors. I can't figure out for the life of me 
//what collection headers actually belong to C. Could have been 
//done with any collection that has automatic resizing.
//
/*
	struct Shapes
	modes - used modes
	shapes - used shapes
	types - used types
	pens - used pens

	Contains all shapes to be drawn. Adds all vectors
	at the same time, so position 1 in each array corresponds
	to the 2nd shape created by the user.
*/
typedef struct Shapes{
	std::vector<int> modes;
	std::vector<RECT> shapes;
	std::vector<int> types;
	std::vector<int> pens;
}Shapes;

/*
 struct drawStr
 currentPen - currently used pen
 currentBG - currently used background
 lastBG - last used background
 currentShape - currently selected shape
 mode- currently selected mode
 dlgHWND - Paint box HWND
 parentHWND - Main window HWND
 Shapes - struct containing all shapes to be drawn

 Contains crucial information for the program, set in
 in the Window variable
*/
typedef struct drawStr{
	int currentPen;
	int currentBG;
	int lastBG;
	int currentShape;
	int mode;
	HWND dlgHWND;
	HWND parentHWND;
	Shapes shp;
}drawStr;

/*
 struct ShapeCopy
 mode - mode of the shape
 type - type of the shape
 pen - pen used for the shape
 shape - the shape itself

 Used to copy and paste a single shape.
*/
typedef struct ShapeCopy{
	int mode;
	int type;
	int pen;
	RECT shape;
}shape;