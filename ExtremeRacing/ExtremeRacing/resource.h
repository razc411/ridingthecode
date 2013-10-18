#include <Windows.h>
#define RACE_CONDITION_TRUE 0x000048

DWORD WINAPI startCar(LPVOID n);
LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM) ;
HWND createCar(LPCWSTR name, int id, int ypos, HWND hwnd, LPARAM lParam);


