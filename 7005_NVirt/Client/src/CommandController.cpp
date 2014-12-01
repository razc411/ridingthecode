/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		CommandController.cpp
--
--	Functions: CommandController()
--             ~CommandController()
--             void check_command(list<TransferController*> * qTransfers)
--             bool file_exists(const std::string& name)
--             void set_descriptor(string line)
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
--	A circular buffer for the storing of connection data.
--
-------------------------------------------------------------------------------------------------------------------------*/
#include "CommandController.h"

extern std::list<TransferController*> transfers;
using namespace std;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: CommandController()
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: CommandController::CommandController()
--
--      RETURNS: Nothing, allows for construction of a CommandController object.
--
--      NOTES:
--      Creates a new CommandController. Sets default values for num packets and window size to 100 and 10 respectively.
----------------------------------------------------------------------------------------------------------------------*/
CommandController::CommandController(): num_packets(100), window_size(10)
{
    log_descriptor.open("log.txt");
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: ~CommandController
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: ~CommandController()
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Destructor for the CommandController. Closes the log file descriptor on call.
----------------------------------------------------------------------------------------------------------------------*/
CommandController::~CommandController()
{
    log_descriptor.close();
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
--      INTERFACE: int CommandController::check_command(list<TransferController*> * qTransfers)
--
--      RETURNS: Nothing.
--
--      NOTES:
--      Reads user input to the terminal and acts on it. Currently accepts input for /send, /quit, /setWindow, /setNumPackets,
--      and /help. Must specify arguments after the command.
----------------------------------------------------------------------------------------------------------------------*/
void CommandController::check_command()
{
    string line, command;
    char delim = ' ';
    stringstream ss;
    getline(cin, line);

    command = line.substr(0, line.find(delim));

    if(command.compare("/quit") == 0)
    {
        exit(1);
    }

    else if(command.compare("/setWindow") == 0)
    {
        string destip = line.substr(11);
    }

    else if(command.compare("/setNumPackets") == 0)
    {

    }

    else if(command.compare("/send") == 0)
    {
        string destip = line.substr(6);

        TransferController * new_transfer = new TransferController(destip, P_SIZE * num_packets, window_size);

        transfers.push_back(new_transfer);
    }

    else if(command.compare("/help") == 0)
    {
        cout << "Welcome to the NVirt dummy client!" << endl;
        cout << "Command Options" << endl;
        cout << "========================================" << endl;
        cout << "/send <ip> : begins a transfer, will be added to queue if transfer currently in progress." << endl;
        cout << "/quit : exits the emulator." << endl;
        cout << "========================================" << endl;
    }
    else
    {
        cout << command << " is not a valid command. Type /help to review the commands." << endl;
    }
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: file_exists
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: bool file_exists(const std::string& name)
--
--      RETURNS: Returns true if file exists, false if it does not.
--
--      NOTES:
--      Checks if a file exists.
----------------------------------------------------------------------------------------------------------------------*/
bool CommandController::file_exists(const std::string& name)
{
    if (FILE *file = fopen(name.c_str(), "r"))
    {
        fclose(file);
        return true;
    }

    return false;
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
void CommandController::set_descriptor(string line)
{
    if(log_descriptor.is_open())
    {
        log_descriptor.close();
    }

    string command = line.substr(5);
    log_descriptor.open(command.c_str());
    cout << "Log file set to " << command << endl;
}
