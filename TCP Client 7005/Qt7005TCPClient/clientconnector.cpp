#include "clientconnector.h"

ClientConnector::ClientConnector(QObject *parent) :
    QObject(parent)
{
}

void ClientConnector::Test()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    qDebug() << "Connecting,..";

    socket->connectToHost("localhost", 3224);

    if(!socket->waitForDisconnected(1000))
    {
        qDebug() << "Error: " << socket->errorString();
    }

}

void ClientConnector::connected()
{
    qDebug() << "Connected!";

    socket->write("Hello");
}

void ClientConnector::disconnected()
{
    qDebug() << "Disconnected!";
}

void ClientConnector::bytesWritten(qint64 bytes)
{
    qDebug() << "We wrote: " << bytes;
}

void ClientConnector::readyRead()
{
    qDebug() << "Reading...";
    qDebug() << socket->readAll();
}
