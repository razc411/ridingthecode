// A simple UCP client program allows the user to specify a server by its domain name.

#include "include/definitions.h"
#include "include/Controller.h"

void *execute(void *placeholder);

int main ()
{
    pthread_t thread;
    int err = 0;

    if((err = pthread_create(&thread, NULL, execute, NULL)))
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
void *execute(void *placeholder)
{
    Controller * network_ctrl = new Controller();
    network_ctrl->execute();

    return NULL;
}

