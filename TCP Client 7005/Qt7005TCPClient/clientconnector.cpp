#include "clientconnector.h"

ClientConnector::ClientConnector(QObject *parent) :
    QThread(parent)
{
}

void ClientConnector::run()
{
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(bytesWritten(qint64)), this, SLOT(bytesWritten(qint64)));

    qDebug() << "Connecting,..";

    socket->connectToHost("localhost", 3224);

    exec();
}

void ClientConnector::connected()
{
    qDebug() << "Connected!";

    socket->write(";T7005PKTFLISTREQ");
    socket->flush();
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
