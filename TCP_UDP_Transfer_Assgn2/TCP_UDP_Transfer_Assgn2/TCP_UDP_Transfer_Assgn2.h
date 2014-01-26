#pragma once

#include "resource.h"

ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
// WndProc called functions
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void				Main_OnPaint(HWND hwnd);
void				Main_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void				Main_OnDestroy(HWND hwnd);
BOOL				Main_OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);