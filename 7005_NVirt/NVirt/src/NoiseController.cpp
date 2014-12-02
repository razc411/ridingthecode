/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		NoiseController.cpp
--
--	Functions: NoiseController()
--             ~NoiseController()
--             void check_command()
--             void set_ploss(string line)
--             void set_descriptor(string line)
--             void set_delay(string line)
--             void print_help()
--             bool packet_drop_check()
--
--	Date:			November 24 2014
--
--	Revisions:
--
--
--	DESIGNERS:		Ramzi Chennafi
--
--
--	PROGRAMMER:		Ramzi Chennafi
--
--	NOTES:
--	Noise controller for the connection. Checks user input commands and applies user commands to the program. Also
--  applies noise functions to incoming data and manages the log.
-------------------------------------------------------------------------------------------------------------------------*/
#include "NoiseController.h"

using namespace std;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: ~NoiseController
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: ~NoiseController()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Destructor for the NoiseController. Closes the log file descriptor on call.
----------------------------------------------------------------------------------------------------------------------*/
NoiseController::NoiseController(): set_logging(true), data_loss(0), packet_delay(0)
{
    log_descriptor.open("log.txt");
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: print_help
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void print_help()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Prints the help message to the terminal.
----------------------------------------------------------------------------------------------------------------------*/
NoiseController::~NoiseController()
{
    log_descriptor.close();
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: packet_drop_check
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: bool packet_drop_check()
--
--      RETURNS: A bool indicating whether the packet should be dropped.
--
--      NOTES:
--      Performs a rand math function based on the currently set packet loss and returns the value.
----------------------------------------------------------------------------------------------------------------------*/
bool NoiseController::packet_drop_check()
{
    double val = rand() % 100;
    return (val > data_loss) ? true : false;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: check_command
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void NoiseController::check_command()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Reads user input to the terminal and acts on it. Currently accepts input for /send, /quit, /setWindow, /setNumPackets,
--      and /help. Must specify arguments after the command.
----------------------------------------------------------------------------------------------------------------------*/
void NoiseController::check_command()
{
    string line, command;
    char delim = ' ';
    getline(cin, line);

    command = line.substr(0, line.find(delim));

    if(command.compare("/quit") == 0)
    {
        exit(1);
    }

    else if(command.compare("/setdelay") == 0)
    {
        set_delay(line);
    }

    else if(command.compare("/setloss") == 0)
    {
        set_ploss(line);
    }

    else if(command.compare("/log") == 0)
    {
        set_descriptor(line);
    }

    else if(command.compare("/help") == 0)
    {
        print_help();
    }
    else
    {
        cout << command << " is not a valid command. Type /help to review the commands." << endl;
    }
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: print_help
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void print_help()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Prints the help message to the terminal.
----------------------------------------------------------------------------------------------------------------------*/
void NoiseController::print_help()
{
    cout << "Welcome to the NVirt network emulator!" << endl;
    cout << "Command Options" << endl;
    cout << "========================================" << endl;
    cout << "/setdelay <millaseconds> : sets the internal packet transfer delay." << endl;
    cout << "/setloss <loss number> : sets loss and takes a percentage. 100 - 0." << endl;
    cout << "/quit : exits the emulator." << endl;
    cout << "/log <filename> : sends all connection data to a log." << endl;
    cout << "========================================" << endl;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: set_delay
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void set_delay(string line)
--                  line - contains the delay set by the user.
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Sets the current packet delay in ms.
----------------------------------------------------------------------------------------------------------------------*/
void NoiseController::set_delay(string line)
{
    string command = line.substr(10);
    stringstream ss;
    ss << command;
    ss >> packet_delay;
    cout << "Delay set to " << packet_delay << "." << endl;
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: set_ploss
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void set_ploss(string line)
--                  line - contains the loss set by the user.
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Sets the packet loss variable for the noise controller.
----------------------------------------------------------------------------------------------------------------------*/
void NoiseController::set_ploss(string line)
{
    int ploss;

    string command = line.substr(9);
    stringstream ss;
    ss << command;
    ss >> ploss;

    if(ploss <= 100 && ploss >= 0)
    {
        data_loss = ploss;
        cout << "Packet loss set to " << data_loss << "." << endl;
    }
    else
    {
        cout << "Delay must be a percentage below or at 100%. Value should be between 0 - 100." << endl;
    }
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: set_descriptor
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: void set_descriptor(string line)
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Sets the log file to be written to.
----------------------------------------------------------------------------------------------------------------------*/
void NoiseController::set_descriptor(string line)
{
    if(log_descriptor.is_open())
    {
        log_descriptor.close();
    }

    string command = line.substr(5);
    log_descriptor.open(command.c_str());
    cout << "Log file set to " << command << endl;
}
