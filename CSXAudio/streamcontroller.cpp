#include "streamcontroller.h"

StreamController::StreamController(QObject *parent) :
    QObject(parent)
{
}

void StreamController::newClient(int socketDescriptor)
{
    QTcpSocket sock = new QTcpSocket;
    sock.setSocketDescriptor(socketDescriptor);
}
