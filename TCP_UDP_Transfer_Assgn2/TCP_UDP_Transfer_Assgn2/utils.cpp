/*--------------------------------------------------------------------------------------------------------------------
--	SOURCE: utils.cpp
--
--	PROGRAM : TCP/UDP Transfer Win32 Assign2
--
--	FUNCTIONS :
--		void set_settings(HWND hwnd)
--		LPSTR grab_file(HWND hwnd, HANDLE * hf)
--		void save_file(HWND hwnd, char * buffer, int size)
--
--	DATE: Febuary 3, 2014
--	REVISIONS : none
--
--	DESIGNER : Ramzi Chennafi
--  PROGRAMMER : Ramzi Chennafi
--
--	NOTES :
--	Contains various utility functions for the program. Performs the opening and saving of files, as well as the network
--	statistics and prints them to their respective file/interface. Also manages the program wide settings.
----------------------------------------------------------------------------------------------------------------------*/
#include "stdafx.h"
#include "TCP_UDP_Transfer_Assgn2.h"
static LPSTR stats = "statistics.txt";
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_server
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: LPSTR grab_file(HWND hwnd, HANDLE * hf), takes the parent window HWND and a pointer to a file handle as arguments.
--
--      RETURNS: A LPSTR containing the file name. Also sets the *hf to the file specified by the user.
--
--      NOTES:
--		Takes the passed in file point and opens the Common Dialog Box so the user can choose a file to send. Sets the file
--		pointer to the file they have chosen and opens it for reading.
----------------------------------------------------------------------------------------------------------------------*/
LPSTR grab_file(HWND hwnd, HANDLE * hf){
	OPENFILENAME ofn;       
	char szFile[260];       
	
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hwnd;
	ofn.lpstrFile = szFile;

	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All\0*.*\0Text\0*.TXT\0";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	if (GetOpenFileName(&ofn) == TRUE){
		*hf = CreateFile(ofn.lpstrFile, GENERIC_READ, 0, NULL, 
			OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		return ofn.lpstrFileTitle;
	}

	activity("Failed to get file handle.\n", EB_STATUSBOX);
	return NULL;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: save_file
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void save_file(HWND hwnd, char * buffer, int size) : takes the parent window HWND, a buffer containing
--		the data received and an int specifying the size of the buffer. Saves the file to the user specified location after
--		it has brought up the common dialog box.
--
--      RETURNS: void
--
--      NOTES:
--      Takes the passed in data and saves it to a file using the Common Dialog Box. User may choose the filename and place
--		to save it using this.
----------------------------------------------------------------------------------------------------------------------*/
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
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: init_server
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void init_server(HWND hwnd) , takes the parent HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      Intializes the server, the type of intialization depends on the chosen protocol in the settings. Listens to the socket
--		whenever the connection is TCP.
----------------------------------------------------------------------------------------------------------------------*/
void set_transfer_stats_server(HWND hwnd, LPSOCKET_INFORMATION SocketInfo, double seconds){
	
	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	char buffer[MAX_SIZE]; 
	char * prot;
	prot = (st->protocol == 0) ? "TCP" : "UDP";
	sprintf_s(buffer, "Recieved %d bytes in %f seconds. %d packets were sent over %s.\n", SocketInfo->totalRecv, seconds, SocketInfo->packets, prot);
	
	HANDLE hFile = CreateFile(stats, FILE_GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE){
		activity("Failed to save statistics.\n", EB_STATUSBOX);
		return;
	}
	
	WriteFile(hFile, buffer, strlen(buffer), NULL, NULL);

	CloseHandle(hFile);

}
void set_transfer_stats_client(HWND hwnd, int lost_packets, int total_size, double seconds, int packet_size){

	SETTINGS * st = (SETTINGS*)GetClassLongPtr(hwnd, 0);
	char buffer[MAX_SIZE];

	if (st->protocol == TCP){
		sprintf_s(buffer, "Sent %d bytes in %f seconds. %d packets were sent over TCP.\n", total_size, seconds, (total_size/packet_size));
	}
	else{
		sprintf_s(buffer, "Sent %d bytes in %f seconds. %d packets were sent over UDP. %d packets were lost.\n", total_size, seconds, (total_size / packet_size), lost_packets);
	}

	HANDLE hFile = CreateFile(stats, FILE_GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (hFile == INVALID_HANDLE_VALUE){
		activity("Failed to save statistics.\n", EB_STATUSBOX);
		return;
	}

	WriteFile(hFile, buffer, strlen(buffer), NULL, NULL);

	CloseHandle(hFile);

}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: set_settings
--
--      DATE: Febuary 6 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void set_settings(HWND hwnd), takes the parent window HWND as an argument.
--
--      RETURNS: void
--
--      NOTES:
--      Whenever the user hits "OK" on the settings dialog box, this function is called. The function will set the current
--		value of the SETTINGS data in the Class Long Ptr to whatever is currently present in the settings dialog box.
----------------------------------------------------------------------------------------------------------------------*/
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

	Edit_GetText(hDlgPORT, tempBuffers[0], MAX_SIZE);
	st->client_port = tempBuffers[0];
	Edit_GetText(hDlgSPORT, tempBuffers[1], MAX_SIZE);
	st->server_port = tempBuffers[1];
	Edit_GetText(hDlgIP, tempBuffers[2], MAX_SIZE);
	st->client_send_ip = tempBuffers[2];
	Edit_GetText(hDlgTTS, tempBuffers[3], MAX_SIZE);
	st->times_to_send = tempBuffers[3];
	st->packet_size = ComboBox_GetCurSel(hDlgPCKT);
	st->protocol = ComboBox_GetCurSel(hDlgSPRTCL);

	SetClassLongPtr(GetParent(hwnd), 0, (LONG)st);
}