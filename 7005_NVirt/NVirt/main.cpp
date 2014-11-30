#include <iostream>
#include <stdio.h>
#include <pthread.h>
#include "include/Controller.h"
#define DEBUG 1
using namespace std;

void *execute(void *quit_request);
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
int main(int argc, char ** argv)
{
    cout << "Welcome to the NVirt network emulator!" << endl;
    cout << "Command Options" << endl;
    cout << "========================================" << endl;
    cout << "/setdelay <millaseconds> : sets the internal packet transfer delay." << endl;
    cout << "/setloss <loss number> : sets loss and takes a percentage. 100 - 0." << endl;
    cout << "/quit : exits the emulator." << endl;
    cout << "/log <filename> : sends all connection data to a log." << endl;
    cout << "========================================" << endl;

    pthread_t thread;
    int err = 0;
    bool quit = false;
    string input;

    if((err = pthread_create(&thread, NULL, execute, (void*)&quit)))
    {
        perror("Failed to create controller thread.");
    }

    pthread_join(thread, NULL);
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
void *execute(void *quit_request)
{
    Controller * mainController = new Controller();
    mainController->execute();

    return NULL;
}
