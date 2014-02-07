/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: Assignment1Term4SPII.cpp
--
--	PROGRAM : Raw Terminal Input through Forks and Pipes
--
--	FUNCTIONS :
--		int main()
--		void process_input(int rawp[2], int transp[2], pid_t * children)
--		void translate_input(int frmtp[2], int transp[2])
--		void print_output(int rawp[2], int frmtp[2])
--		void fatal_error(char * error)
--
--	DATE: January 20, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	A program which disables terminal text processing and handles it instead.Several methods of character input have
--	changed, as well as methods of terminating the program.Prints out one line of raw input, then upon typing "E",
--	prints out a formatted line of text.All 'a' characters are changed to 'z', all 'z' characters are changed to 'a',
--	and control letters such as X, E, K and T are not printed in the formatted text.
--
--
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"

LPSTR grab_file(HWND hwnd, HANDLE * hf){
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	// Initialize OPENFILENAME
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	// Display the Open dialog box. 

	if (GetOpenFileName(&ofn) == TRUE){
		*hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		return ofn.lpstrFileTitle;
	}

	MessageBox(hwnd, "Failed to get file handle", "Error", MB_ICONEXCLAMATION);
	return NULL;
}

void save_file(HWND hwnd, char * buffer, int size){
	TCHAR   szFile[MAX_PATH] = TEXT("\0");
	OPENFILENAME   ofn;
	HANDLE hFile = INVALID_HANDLE_VALUE;
	DWORD bytesWritten = 0;

	memset(&(ofn), 0, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = TEXT("Text (*.txt)\0*.txt\0");
	ofn.lpstrTitle = TEXT("Save File As");
	ofn.Flags = OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("txt");

	if (!GetSaveFileName(&ofn))
	{
		MessageBox(hwnd, "Failed to save file.", "Error", MB_ICONEXCLAMATION);
		return;
	}

	hFile = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE){
		MessageBox(hwnd, "Failed to save file.", "Error", MB_ICONEXCLAMATION);
		return;
	}

	WriteFile(hFile, buffer, size, &bytesWritten, NULL);

	CloseHandle(hFile);
}

void net_stats(){

}

void set_settings(HWND hwnd){

	char ** tempBuffers = (char**)malloc(sizeof(char*)* 4);
	for (int i = 0; i < 4; i++){
		tempBuffers[i] = (char*) malloc(sizeof(char)* MAX_SIZE);
	}

	HWND hDlgPORT = GetDlgItem(hwnd, IDC_PORT);
	HWND hDlgIP = GetDlgItem(hwnd, IDC_IP);
	HWND hDlgSAVE = GetDlgItem(hwnd, IDC_SDISPLAY);
	HWND hDlgSPORT = GetDlgItem(hwnd, IDC_SPORT);
	HWND hDlgSPRTCL = GetDlgItem(hwnd, IDC_SPRTCL);
	HWND hDlgPCKT = GetDlgItem(hwnd, IDC_PACKETSIZE);
	HWND hDlgTTS = GetDlgItem(hwnd, IDC_TTS);

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(GetParent(hwnd), 0);

	Edit_GetText(hDlgPORT, tempBuffers[0], 6);
	st->client_port = tempBuffers[0];
	Edit_GetText(hDlgSPORT, tempBuffers[1], 6);
	st->server_port = tempBuffers[1];
	Edit_GetText(hDlgIP, tempBuffers[2], 17);
	st->client_send_ip = tempBuffers[2];
	Edit_GetText(hDlgTTS, tempBuffers[3], 3);
	st->times_to_send = tempBuffers[3];
	st->packet_size = ComboBox_GetCurSel(hDlgPCKT);
	st->protocol = ComboBox_GetCurSel(hDlgSPRTCL);

	SetClassLongPtr(GetParent(hwnd), 0, (LONG)st);
}