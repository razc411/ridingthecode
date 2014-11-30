// A simple UCP client program allows the user to specify a server by its domain name.

#include <stdio.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <strings.h>
#include <pthread.h>
#include <string.h>
#include "include/Controller.h"

#define OPTIONS "f:p:t:RS"

void *execute(void *placeholder);

int main (int argc, char **argv)
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

