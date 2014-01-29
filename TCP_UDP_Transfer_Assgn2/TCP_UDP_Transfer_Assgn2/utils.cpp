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

void net_stats(){

}

void set_settings(HWND hwnd){
	HWND hDlgPTCL	= GetDlgItem(hwnd, IDC_PROTSLT);
	HWND hDlgPORT	= GetDlgItem(hwnd, IDC_PORT);
	HWND hDlgIP		= GetDlgItem(hwnd, IDC_IP);
	HWND hDlgSAVE	= GetDlgItem(hwnd, IDC_SDISPLAY);
	HWND hDlgSPORT	= GetDlgItem(hwnd, IDC_SPORT);
	HWND hDlgSPRTCL = GetDlgItem(hwnd, IDC_SPRTCL);

	SETTINGS * st = (SETTINGS*) malloc(sizeof(SETTINGS));
	
	Edit_GetText(hDlgPORT, st->client_port, MAX_SIZE);
	Edit_GetText(hDlgSPORT, st->server_port, MAX_SIZE);

	st->client_prtcl = ComboBox_GetCurSel(hDlgPTCL);
	st->server_prtcl = ComboBox_GetCurSel(hDlgSPRTCL);
	Edit_GetText(hDlgIP, st->client_send_ip, MAX_SIZE);

	SetClassLongPtr(GetParent(hwnd), 0, (LONG)st);
}