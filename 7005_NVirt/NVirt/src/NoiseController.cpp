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
#include "NoiseController.h"

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
NoiseController::NoiseController(): set_logging(true), data_loss(0), packet_delay(0)
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
NoiseController::~NoiseController()
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
bool NoiseController::packet_drop_check()
{
    double val = rand() % 100;
    return (val > packet_delay) ? true : false;
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
int NoiseController::check_command()
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
        cout << "Welcome to the NVirt network emulator!" << endl;
        cout << "Command Options" << endl;
        cout << "========================================" << endl;
        cout << "/setdelay <millaseconds> : sets the internal packet transfer delay." << endl;
        cout << "/setloss <loss number> : sets loss and takes a percentage. 100 - 0." << endl;
        cout << "/quit : exits the emulator." << endl;
        cout << "/log <filename> : sends all connection data to a log." << endl;
        cout << "========================================" << endl;
    }
    else
    {
        cout << command << " is not a valid command. Type /help to review the commands." << endl;
    }

    return 1;
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
void NoiseController::set_delay(string line)
{
    string command = line.substr(10);
    stringstream ss;
    ss << command;
    ss >> packet_delay;
    cout << "Delay set to " << packet_delay << "." << endl;
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
