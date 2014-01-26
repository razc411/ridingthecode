/*------------------------------------------------------------------------------------------------------------------
--		SOURCE FILE:	serialPhys.cpp
--		PROGRAM:		DumbTerminalEmulator
--		Functions
--			init(ioStruct *io)
--			execRead(LPVOID ioS)
			writeOutputBuffer(HANDLE hdSerial, WPARAM chBuffer)
			void endSession(HANDLE hdSerial, HANDLE readThrd)
--
--		DATE:			September 21st, 2013
--		DESIGNER:		Ramzi Chennafi	
--		PROGRAMMER:		Ramzi Chennafi
--
--		NOTES:
--		Physical Layer of the program. Opens the serial port for communication, then provides functions that can
		read and write to that serial port. A new thread is created for the read portion of this layer. Also preforms
		buffer flushing and session termination.
----------------------------------------------------------------------------------------------------------------------*/
#include <Windows.h>
#include "resource.h"
/*----------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		init
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		HANDLE init(ioStruct *io)				
--
--		RETURNS:		The port handle on successful opening of connection and NULL on failure, along with a 
						warning message on failure.
--
--		NOTES:
--		Opens the selected serial port in synchronous i/o mode. 
----------------------------------------------------------------------------------------------------------------------*/

HANDLE init(ioStruct *io){

			HANDLE hdSerial = CreateFile(io->port, 
								  GENERIC_READ | GENERIC_WRITE, 
								  0, 
								  0, 
							      OPEN_EXISTING, 
								  0, 
							      0);

			if(hdSerial == INVALID_HANDLE_VALUE){
				MessageBox (io->hwnd, TEXT("Failed to Connect"), TEXT("Dumb Terminal"), MB_OK);
				return NULL;
			}
			
			SetCommState(hdSerial, &io->cc.dcb);

			return hdSerial;
	}
/*------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		execRead
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		DWORD WINAPI execRead(LPVOID ioS)						
--
--		RETURNS:		DWORD exit code
--
--		NOTES:
--		New thread function called within the wndProc. Deals with the reading/formatting of recieved text and its display. Recieves an
		ioStruct containing the serial port handle and the window HWND.
----------------------------------------------------------------------------------------------------------------------*/
	DWORD WINAPI execRead(LPVOID ioS){
		
	WCHAR			charBuff[2] = TEXT("");
	DWORD			bytesRead	= 0;
	static unsigned k			= 0;
	struct ioStruct	*io			= (struct ioStruct *)ioS;
	HDC				hdc			= GetDC(io->hwnd);
	TEXTMETRIC		tm;
	int				Y			= 0;
	int				X			= 0;
	GetTextMetrics(hdc, &tm);

	while(1){	
		if(io->hdSerial != NULL){	
			if(ReadFile(io->hdSerial, charBuff, 1, &bytesRead, NULL)){
				charBuff[1] = '\0';
				if(X >= WIN_WIDTH){
					Y = Y + tm.tmHeight + tm.tmExternalLeading;
					X = 0;
				}
				TextOut(hdc, X, Y, charBuff, 2); 
				X += tm.tmMaxCharWidth;
			}
		}
	}

	ReleaseDC(io->hwnd, hdc);	
	return 0;
}
/*------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		writeOutputBuffer
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		writeOutputBuffer(HANDLE hdSerial, WPARAM chBuffer)					
--
--		RETURNS:		bool, true on completed write operation
--
--		NOTES:
--		Writes the character provided to a file. A port file in this case.
----------------------------------------------------------------------------------------------------------------------*/
bool writeOutputBuffer(HANDLE hdSerial, WPARAM chBuffer){
	
	DWORD			bytesWritten = 0;
	
	if(WriteFile(hdSerial, &chBuffer, 1, &bytesWritten, NULL)){
		return true;
	}
	return false;
}
/*------------------------------------------------------------------------------------------------------------------
--		FUNCTION:		endSession
--		DATE:			September 21st, 2012
--		REVISIONS:		n/a
--		DESIGNER:		Ramzi Chennafi
--		PROGRAMMER:		Ramzi Chennafi	
--
--		INTERFACE:		endSession(HANDLE hdSerial, HANDLE readThrd)					
--
--		RETURNS:		Nothing
--
--		NOTES:
--		Flushes the I/O buffer, closes the serial port and terminates the read thread. Will not do so unless
		handles have been intiated.
----------------------------------------------------------------------------------------------------------------------*/
void endSession(HANDLE hdSerial, HANDLE readThrd){
	
	if(hdSerial != NULL && readThrd != NULL){
		CancelSynchronousIo(readThrd);
		CloseHandle(hdSerial);
		TerminateThread(readThrd, 0);
	}
}