COMP4981 Assignment 3
Author: Tim Kim   Ramzi Chennafi
Date: March 24, 2014
Set: 4O (Data Comm)

-----------------FEATURES--------------------------------------

Included in this package are a linux client and and server, for chatting over TCP.

The server is capable of creating multiple channels and maintaining users in these channels.
Each client may only join one channel however, but anyone who is in the same channel
will be able to chat with that user and will not hear chat from other channels.

-----------------COMPILING AND RUNNING--------------------------------------
REQUIRED: C++, to compile from scratch G++
		  A linux distribution
TO COMPILE:
	
TO RUN:
	Server
	- In the UNIX terminal, navigate to the bin folder in the server folder
	and execute the server by typing ./client_chat
	- Now simply create channels and wait to process those clients
	

	Client
	- In the UNIX terminal, navigate to the bin folder in the client folder
	and execute the chat client by ./client_chat [username] [ip]
	- Now simply join channels and chat away

-----------------INSTRUCTIONS----------------------------------------------
To run the server,
- Type /create [channelname] to create a chat channel
- Type /close [channelname] to close the chat channel
- Type /users to display current users
- Type /exit to exit the program

To run the client,

- Type /join [channelname] to join the chat channel
- Type /leave [channelname] to leave the chat channel
- Type /log to begin logging chat to chatlog.txt
- Type /exit to exit the program

-----------------INCLUDED FILES---------------------------------------------
There are three source files and three header in server/src folder. They are
- ChannelSystem.cpp
- ServerSystemRouter.cpp
- server_defs.h
- packets.h
- utils.cpp
- utils.h

There are three source files and three header in client/src folder. They are
- InputController.cpp
- SystemRouter.cpp
- client_defs.h
- packets.h
- utils.cpp
- utils.h

-----------------DOCUMENTATION----------------------------------------------
DocumentationPaper.pdf is also included and it contains all the design work
including State Diagram, Pseudocode, and testing documents.

