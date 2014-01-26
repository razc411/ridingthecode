/*------------------------------------------------------------------------------------------------------------------
--		SOURCE FILE:	serialApp.cpp
--		PROGRAM:		DumbTerminalEmulator
--		Functions
--				WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
-- 						  LPSTR lspszCmdParam, int nCmdShow)	
--				LRESULT CALLBACK WndProc (HWND, UINT, WPARAM, LPARAM);
--				void checkItem(HMENU, UINT);
--
--		DATE:			September 21st, 2013
--		DESIGNER:		Ramzi Chennafi	
--		PROGRAMMER:		Ramzi Chennafi
--
--		NOTES:
--		Application layer of the program. Deals with graphical components and drawing, excluding the external thread draw.
--
--		Handles the window of the terminal. Also deals with intiating read and writes along the serial connection
--		and the settings that go with said serial connection. Prints any data recieved to the window and sends any keyboard input
--		through the serial line after the Connect menu item is pressed. 
----------------------------------------------------------------------------------------------------------------------*/
#include <windows.h>
#include "resource.h"
#include <cstdio>
HANDLE			hComm;
static TCHAR	Name[]		= TEXT("serialTerm");
ioStruct *io = (struct ioStruct *)malloc(sizeof(ioStruct));

/*------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		WinMain
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
--											LPSTR lspszCmdParam, int nCmdShow)					
--
--		RETURNS:		int 
--
--		NOTES:
--		Creates a new window for the application and begins calls. Intiates hwnd, port and serialOn trigger of the ioStruct.
----------------------------------------------------------------------------------------------------------------------*/

int WINAPI WinMain (HINSTANCE hInst, HINSTANCE hprevInstance,
 						  LPSTR lspszCmdParam, int nCmdShow)
{
	HWND		hwnd;
	MSG			Msg;
	WNDCLASSEX	Wcl;
	
	Wcl.cbSize = sizeof (WNDCLASSEX);
	Wcl.style = CS_HREDRAW | CS_VREDRAW;
	Wcl.hIcon = LoadIcon(NULL, IDI_APPLICATION); 
	Wcl.hIconSm = NULL; 
	Wcl.hCursor = LoadCursor(NULL, IDC_ARROW);  
	
	Wcl.lpfnWndProc = WndProc;
	Wcl.hInstance = hInst;
	Wcl.hbrBackground = (HBRUSH) GetStockObject(WHITE_BRUSH); 
	Wcl.lpszClassName = Name;
   
	Wcl.lpszMenuName = TEXT("MYMENU");
	Wcl.cbClsExtra = 0;   
	Wcl.cbWndExtra = 0;
	
	if (!RegisterClassEx (&Wcl))
		return 0;
   
	hwnd = CreateWindow (Name, Name, WS_OVERLAPPEDWINDOW, 350, 160,
   							800, 500, NULL, NULL, hInst, NULL);
	io->serialOn = false;
	io->port = TEXT("com1");
	io->hwnd = hwnd;
	
	ShowWindow (hwnd, nCmdShow);
	UpdateWindow (hwnd);

	while (GetMessage (&Msg, NULL, 0, 0))
	{
   		TranslateMessage (&Msg);
		DispatchMessage (&Msg);
	}

	return Msg.wParam;
}
/*------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		WndProc
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
--                          WPARAM wParam, LPARAM lParam)				
--
--		RETURNS:		LRESULT to the system.
--
--		NOTES:
--		Deals with the main window thread. Requests writes, modification of data settings, usage information and 
--		connect/disconnect buttons. Also creates a new thread to perform read operations.
----------------------------------------------------------------------------------------------------------------------*/
LRESULT CALLBACK WndProc (HWND hwnd, UINT Message,
                          WPARAM wParam, LPARAM lParam)
{
	HMENU			menuHnd	= GetMenu(hwnd);
	
	switch (Message)
   {
		case WM_COMMAND:
		   switch (LOWORD (wParam))
			{
				case IDM_CONNECT:
					if(!io->serialOn){
						if((io->hdSerial = init(io)) != NULL){
							MessageBox (hwnd, TEXT("Client Connected"), TEXT("Dumb Terminal"), MB_OK | MB_ICONEXCLAMATION);
							io->serialOn = true;
							io->readThrd = CreateThread(NULL, 0, execRead, io, 0, &io->threadId);
							EnableMenuItem(menuHnd, IDM_CONNECT, MF_GRAYED);
							DrawMenuBar(hwnd);
						}
					}
				break;

				case IDM_HELP:
					MessageBox (hwnd, TEXT("Choose which port you wish to communicate on and then hit data settings to modify the communication settings.\nConnect will begin a session, hit escape to end the current session.\nAny keyboard input will be transferred during a session."), 
						TEXT("Dumb Terminal"), MB_OK | MB_ICONINFORMATION);
				break;
				
				case ID_PORT_COMM1:
					io->port = TEXT("com1");
					checkItem(menuHnd, COMM1);
				break;
				
				case ID_PORT_COMM3:
					io->port = TEXT("com3");
					checkItem(menuHnd, COMM3);
				break;

          		case IDM_DATA_SETTINGS:
            		io->cc.dwSize = sizeof(COMMCONFIG);
					io->cc.wVersion = 0x100;
					GetCommConfig (hComm, &io->cc, &io->cc.dwSize);
            		if (!CommConfigDialog (io->port, io->hwnd, &io->cc))
               			break;
				break;
			}
		break;

		case WM_CHAR:	
			if(io->serialOn && wParam != VK_ESCAPE){
				writeOutputBuffer(io->hdSerial, wParam);
			}
		break;

		case WM_KEYDOWN:
			if(wParam == VK_ESCAPE && io->serialOn == true){
				io->serialOn = false;
				
				MessageBox (hwnd, TEXT("Client Disconnected"), TEXT("Dumb Terminal"), MB_OK | MB_ICONEXCLAMATION);
				InvalidateRect(hwnd, NULL, FALSE);
				EnableMenuItem(menuHnd, IDM_CONNECT, MF_ENABLED);
				DrawMenuBar(hwnd);
				
				endSession(io->hdSerial, io->readThrd);
			}
		break;

		case WM_DESTROY:	
      		PostQuitMessage (0);
		break;
		
		default:
			 return DefWindowProc (hwnd, Message, wParam, lParam);
	}
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		checkItem
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		void checkItem(HMENU hnd, UINT port)				
--
--		RETURNS:		Nothing
--
--		NOTES:
--		Check marks the currently used port in the ports menu of the main window. hnd is the menu handle of the item, 
--		while port specifies which item to check. Options are COMM1 and COMM3.
----------------------------------------------------------------------------------------------------------------------*/
	void checkItem(HMENU hnd, UINT port){
		switch(port){
			case COMM1 : 
				CheckMenuItem(hnd, ID_PORT_COMM1, MF_CHECKED);
				CheckMenuItem(hnd, ID_PORT_COMM3, MF_UNCHECKED);
			break;
			case COMM3: 
				CheckMenuItem(hnd, ID_PORT_COMM3, MF_CHECKED);
				CheckMenuItem(hnd, ID_PORT_COMM1, MF_UNCHECKED);
			break;
		}
	}


