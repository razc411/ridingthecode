#include "connectionmanager.h"

ConnectionManager::ConnectionManager(QString dir, QObject *parent) :
    QTcpServer(parent)
{
     fManager = new FileManager(dir);
}

void ConnectionManager::StartServer()
{
    if(!this->listen(QHostAddress::Any,3224))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening...";
    }
}

void ConnectionManager::incomingConnection(qintptr socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";
    ClientHandler *thread = new ClientHandler(socketDescriptor,  fManager, this);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

