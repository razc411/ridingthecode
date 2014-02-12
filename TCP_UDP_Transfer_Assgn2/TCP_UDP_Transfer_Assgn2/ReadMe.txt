========================================================================
    WIN32 APPLICATION : TCP_UDP_Transfer_Assgn2 Project Overview
========================================================================
A UDP/TCP File/packet transfering program. User starts up the client on two computers, one in server and the other in client.
By Ramzi Chennafi

Features
- File Transfer
- Garbage Packet transfer
- Network statistics collection
- TCP protocol
- Somewhat-Reliable UDP protocol (Dropped packets will be resent)

Issues
- Can freeze and have the connection bug out when trying to disconnect a TCP connection.
- Cannot be used on localhost

Things to Know when using this program:

- Maximum transfer size = 2000000 * 60KB = 120 Gigabytes. Untested on files above 1 Gigabyte. Expect huge slowdown transfering large files with UDP.
- UDP transfer is somewhat reliable, and will resend dropped packets. Gives no consideration for pack order, which shouldn't be an issue on an uncrowded LAN.
- Both clients need the ip of the other client put into the settings. 
- The client socket on the client side must be set to the same port as the server port on the server side. The same can be said for the server port on the
- The client side and the client port on the server side.
- On a TCP connection, after both the server and client have been intialized, the client must "Connect" with the server. This can be done using the obvious button.
- UDP does not require the user to "Connect". Simply intialize each side.
- Program is single threaded and asynchronous. Spent too much time on other considerations. It will hang while transferring data.
- The packet size setting will also affect the packet size of file transfers, and not just garbage transfers.
- As it stands, program is untested on WAN (I live at BCIT, makes sending this stuff out on a WAN difficult).
- Due to implementation, this program cannot do UDP on localhost.

Compiled on windows using Visual Studio 2010. Only tested on Windows 7. 