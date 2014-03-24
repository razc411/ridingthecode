COMP4981 Assignment 3
Author: Tim Kim   Ramzi Chennafi
Date: March 24, 2014
Set: 4O (Data Comm)

INSTRUCTION-----------------------------------------------------------------
To run the server,
- In the UNIX terminal, run server_chat in server/bin folder
	(You might have to change permission)
- Type /create [channelname] to create a chat channel
- Type /close [channelname] to close the chat channel
- Type /user to display current users
- Type /exit to exit the program

To run the client,
- In the UNIX terminal, run client_chat in client/bin folder
	(You might have to change permission)
- Type /join [channelname] to join the chat channel
- Type /leave [channelname] to leave the chat channel
- Type /log to display current users
- Type /exit to exit the program

INCLUDED FILES--------------------------------------------------------------
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

DOCUMENTATION---------------------------------------------------------------
DocumentationPaper.pdf is also included and it contains all the design work
including State Diagram, Pseudocode, and testing documents.
