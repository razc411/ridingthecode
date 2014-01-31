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

void grab_file(HWND hwnd){
	OPENFILENAME ofn;       // common dialog box structure
	char szFile[260];       // buffer for file name
	HANDLE hf;              // file handle
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

	if (GetOpenFileName(&ofn) == TRUE)
		hf = CreateFile(ofn.lpstrFile,
		GENERIC_READ,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)NULL);

	st->save_location = ofn.lpstrFile;
	SetClassLongPtr(hwnd, 0, (LONG)st);
}

void net_stats(){

}

void set_settings(HWND hwnd){
	
	HWND hDlgPTCL = GetDlgItem(hwnd, IDC_PROTSLT);
	HWND hDlgPORT = GetDlgItem(hwnd, IDC_PORT);
	HWND hDlgIP = GetDlgItem(hwnd, IDC_IP);
	HWND hDlgSAVE = GetDlgItem(hwnd, IDC_SDISPLAY);
	HWND hDlgSPORT = GetDlgItem(hwnd, IDC_SPORT);
	HWND hDlgSPRTCL = GetDlgItem(hwnd, IDC_SPRTCL);

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);

	Edit_GetText(hDlgPORT, st->client_port, MAX_SIZE);
	Edit_GetText(hDlgSPORT, st->server_port, MAX_SIZE);
	Edit_GetText(hDlgIP, st->client_send_ip, MAX_SIZE);

	st->client_prtcl = ComboBox_GetCurSel(hDlgPTCL);
	st->server_prtcl = ComboBox_GetCurSel(hDlgSPRTCL);

	SetClassLongPtr(GetParent(hwnd), 0, (LONG)st);
}