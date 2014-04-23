#include "tcpacceptor.h"
/**
 * @brief TCPAcceptor::run
 *
 * The run method of the TCPAcceptor thread. Connects slots and signals,
 * then listens for incoming connections.
 */
void TCPAcceptor::run()
{
    connect(this, SIGNAL(newClient(int)), StreamController, SLOT(newClient(int)));
    this->listen("127.0.0.1", 4342);

    while(this->isListening()){}
}

/**
 * @brief TCPAcceptor::incomingConnection
 * @param socketDescriptor
 *
 * Overrides the default incoming connection method to allow for passing
 * of socket descriptor to the StreamController thread for processing.
 */
void TCPAcceptor::incomingConnection(int socketDescriptor)
{
    emit signal(newClient(socketDescriptor));
}
