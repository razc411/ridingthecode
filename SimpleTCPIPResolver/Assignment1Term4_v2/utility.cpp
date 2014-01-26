//-------------------------------------------------------------------------------------------------------
//	PROJECT: TCP/IP Resolver
//	FILE: utility.cpp
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	FUNCTIONS: 
//			int resolve_IP(HWND hwnd)
//			int resolve_host(HWND hwnd)
//			int resolve_service(HWND hwnd)
//			int resolve_port(HWND hwnd)
//			void StartWinsock()
//			int getProtocol(HWND protocl_cb, TCHAR * prtclBuff)
//			void phosterr(HWND text_box)
//	DESCRIPTION:
//			Contains functions for resolving IPs, host names, services and ports. Called by the main loop
//			whenever the corresponding OK button is hit.
//	NOTES:
//			Requires WS2_32.lib for winsock. Character set must be "not set", TCHAR -> char* conversion 
//			problems will arise.
//-------------------------------------------------------------------------------------------------------
#include "stdafx.h"
#include "Assignment1Term4_v2.h"
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: int resolve_IP(HWND hwnd)
//	RETURNS: int, 1 on invalid IP provided, 2 when no host can be found for the IP and 0 on success.
//	DESCRIPTION:
//			Resolves the host belonging to the provided IP. Prints the results to the text_box.
//-------------------------------------------------------------------------------------------------------
int resolve_IP(HWND hwnd){
	
	struct	hostent *hp;
	struct	in_addr my_addr, *addr_p;
	char	**p;
	TCHAR ipBuff[MAX_IP_SIZE];

	HWND ip_ctrl = GetDlgItem(hwnd, ID_IP_CTRL);
	HWND text_box = GetDlgItem(hwnd, ID_EDIT_BOX);
	GetWindowText(ip_ctrl, ipBuff, MAX_IP_SIZE);

	StartWinsock();

	addr_p = (struct in_addr*)malloc(sizeof(struct in_addr));
	addr_p = &my_addr;

	if (inet_addr((char*)ipBuff) == 0)
	{
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"IP Address must be of the form x.x.x.x");
		return 1;
	}
	
	addr_p->s_addr = inet_addr((char*)ipBuff);

	if ((hp = gethostbyaddr((char *)addr_p, PF_INET, sizeof (my_addr))) == NULL)
	{
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Host information for IP not found");
		return 2;
	}
	
	for (p = hp->h_addr_list; *p != 0; p++)
	{
		struct in_addr in;
		char **q;

		memcpy(&in.s_addr, *p, sizeof (in.s_addr));
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"IP Resolved");
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)hp->h_name);
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)inet_ntoa(in));

		for (q = hp->h_aliases; *q != 0; q++) // prints aliases
			SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)*q);
	}

	WSACleanup();
	return 0;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: int resolve_host(HWND hwnd)
//	RETURNS: int, 1 on failure to retrieve host IP, 0 on success.
//	DESCRIPTION:
//			Resolves the IPs belonging to the provided host. Prints the results to the text_box.
//-------------------------------------------------------------------------------------------------------
int resolve_host(HWND hwnd){
	
	struct	hostent *hp;
	char	**p;
	TCHAR hostBuff[MAX_IP_SIZE];

	HWND host_ctrl = GetDlgItem(hwnd, ID_HST_CTRL);
	HWND text_box = GetDlgItem(hwnd, ID_EDIT_BOX);
	GetWindowText(host_ctrl, hostBuff, MAX_IP_SIZE);

	StartWinsock();

	if ((hp = gethostbyname(hostBuff)) == NULL){
		phosterr(text_box);
		return 1;
	}
	
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Host Resolved");
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)hp->h_name);

	for (p = hp->h_addr_list; *p != 0; p++)
	{
		struct in_addr in;
		char **q;

		memcpy(&in.s_addr, *p, sizeof (in.s_addr));
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)inet_ntoa(in));

		for (q = hp->h_aliases; *q != 0; q++) // prints aliases
			SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)*q);
	}

	WSACleanup();
	return 0;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: int resolve_service(HWND hwnd)
//	RETURNS: int, 1 when no protocol is selected, 2 when the port cannot be resolved and 0 on success.
//	DESCRIPTION:
//			Resolves a port from the protocol and service provided by the user. Prints to the text_box.
//-------------------------------------------------------------------------------------------------------
int resolve_service(HWND hwnd){

	struct servent *sv;
	TCHAR servBuff[MAX_LOADSTRING], prtclBuff[PTCLSIZE];
	TCHAR* prtbuff = (TCHAR*) malloc(sizeof(TCHAR));

	HWND service_ctrl = GetDlgItem(hwnd, ID_SRV_CTRL);
	HWND protocl_cb = GetDlgItem(hwnd, ID_SRV_CB);
	HWND text_box = GetDlgItem(hwnd, ID_EDIT_BOX);

	GetWindowText(service_ctrl, servBuff, MAX_LOADSTRING);
	
	if (getProtocol(protocl_cb, prtclBuff) == 0){
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"No protocol selected");
		return 1;
	}

	StartWinsock();

	if ((sv = getservbyname(servBuff, prtclBuff)) == NULL){
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Error in getservbyname");
		return 2;
	}
	wsprintf(prtbuff, "%d", ntohs(sv->s_port));
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Service Resolved");
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)servBuff);
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)prtbuff);

	WSACleanup();
	return 0;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: int resolve_port(HWND hwnd)
//	RETURNS: int, 1 when no protocol is selected, 2 when the service cannot be resolved and 0 on success
//	DESCRIPTION:
//			Resolves a service from the user provided port and protocol. Prints to the text_box.
//-------------------------------------------------------------------------------------------------------
int resolve_port(HWND hwnd){

	struct servent *sv;
	int s_port;
	TCHAR portBuff[MAX_IP_SIZE], prtclBuff[PTCLSIZE];

	HWND port_ctrl = GetDlgItem(hwnd, ID_PRT_CTRL);
	HWND protocl_cb = GetDlgItem(hwnd, ID_PRT_CB);
	HWND text_box = GetDlgItem(hwnd, ID_EDIT_BOX);

	GetWindowText(port_ctrl, portBuff, MAX_LOADSTRING);

	if (getProtocol(protocl_cb, prtclBuff) == 0){
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"No protocol selected");
		return 1;
	}
	
	s_port = atoi(portBuff);

	StartWinsock();

	if ((sv = getservbyport(htons(s_port), prtclBuff)) == NULL)
	{
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Error in getservbyport");
		return 2;
	}
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Port Resolved");
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)portBuff);
	SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)sv->s_name);

	WSACleanup();
	return 0;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: void StartWinsock()
//	RETURNS: Nothing
//	DESCRIPTION:
//			Intiates the winsock asynchronous TCP/IP connection.
//-------------------------------------------------------------------------------------------------------
void StartWinsock(){
	WORD wVersionRequested = MAKEWORD(2, 2);
	WSADATA wsaData;

	WSAStartup(wVersionRequested, &wsaData);
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: int getProtocol(HWND protocl_cb, TCHAR * prtclBuff)
//	RETURNS: int, 1 on TCP, 2 on UDP and 0 when the user fails to choose a protocol.
//	DESCRIPTION:
//			Checks which protocol was chosen in the specified combo box.
//-------------------------------------------------------------------------------------------------------
int getProtocol(HWND protocl_cb, TCHAR * prtclBuff){
	int lResult = -1;
	
	if ((lResult = SendMessage(protocl_cb, CB_GETCURSEL, 0, 0)) == 0){
		wsprintf(prtclBuff, "TCP");
		return 1;
	}
	if (lResult == 1){
		wsprintf(prtclBuff, "UDP");
		return 2;
	}

	return 0;
}
//-------------------------------------------------------------------------------------------------------
//	DATE: January 11 2014
//	AUTHOR: RAMZI CHENNAFI
//	
//	FUNCTION: void phosterr(HWND text_box)
//	RETURNS: Nothing
//	DESCRIPTION:
//			Prints error messages when the program fails to resolve a host name.
//	NOTES:
//			Prints to the text_box HWND, as long as its a Listbox.
//-------------------------------------------------------------------------------------------------------
void phosterr(HWND text_box){
	switch (h_errno)
	{
	case HOST_NOT_FOUND:
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Host not found");
		break;
	case TRY_AGAIN:
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Try again later");
		break;
	case NO_RECOVERY:
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"DNS Error");
		break;
	case NO_ADDRESS:
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Host has no address");
		break;
	default:
		SendMessage(text_box, LB_ADDSTRING, 0, (LPARAM)"Unknown Error");
		break;
	}
}