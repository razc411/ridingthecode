/*----------------------------------------------------------------------------------------------------------------------
--	Source File:		CircularBuffer.cpp
--
--	Functions: size_t size() const { return size_; }
--             size_t capacity() const { return capacity_; }
--             size_t write(const char *data, size_t bytes);
--             size_t read(char *data, size_t bytes);
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

using namespace std;
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
CommandController::CommandController(): num_packets(100), window_size(10)
{
    log_descriptor.open("log.txt");
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
CommandController::~CommandController()
{
    log_descriptor.close();
}
/*------------------------------------------------------------------------------------------------------------------
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
----------------------------------------------------------------------------------------------------------------------*/
int CommandController::check_command(queue<TransferController*> &qTransfers)
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

    if(command.compare("/setWindow") == 0)
    {
        string destip = line.substr(11);
    }

    if(command.compare("/setNumPackets") == 0)
    {

    }

    if(command.compare("/send") == 0)
    {
        string destip = line.substr(6);

        TransferController * transfer = new TransferController(destip.c_str(), P_SIZE * num_packets, window_size);

        qTransfers.push(transfer);
    }

    else if(command.compare("/help") == 0)
    {
        cout << "Welcome to the NVirt dummy client!" << endl;
        cout << "Command Options" << endl;
        cout << "========================================" << endl;
        cout << "/send <filename> : begins a transfer, will be added to queue if transfer currently in progress." << endl;
        cout << "/quit : exits the emulator." << endl;
        cout << "========================================" << endl;
    }
    else
    {
        cout << command << " is not a valid command. Type /help to review the commands." << endl;
    }

    return 1;
}

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
--      FUNCTION: read
--
--      DATE: November 24 2014
--      REVISIONS: none
--
--      DESIGNER: Ramzi Chennafi
--      PROGRAMMER: Ramzi Chennafi
--
--      INTERFACE: read(const char * data, size_t bytes)
--
--      RETURNS: A size_t of the amount of data read.
--
--      NOTES:
--      Reads an amount of data specified by bytes. If empty, returns 0. If requested bytes is larger than the buffer,
--      returns an entire buffer.
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
