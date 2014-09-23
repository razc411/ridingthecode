#include "connectionmanager.h"

ConnectionManager::ConnectionManager(QObject *parent) :
    QTcpServer(parent)
{
}

void ConnectionManager::StartServer(QString * dir)
{
    fManager.setDirectory(dir);

    if(!this->listen(QHostAddress::Any,3224))
    {
        qDebug() << "Could not start server";
    }
    else
    {
        qDebug() << "Listening...";
    }
}

void ConnectionManager::incomingConnection(int socketDescriptor)
{
    qDebug() << socketDescriptor << " Connecting...";
    ClientHandler *thread = new ClientHandler(socketDescriptor, this, &fManager);
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    thread->start();
}

