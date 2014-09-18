#include clienthandler.h"

ClientHandler::ClientHandler(int ID, QObject *parent) :
    QThread(parent)
{
    this->socketDescriptor = ID;
}

void ClientHandler::run()
{
    // thread starts here
    qDebug() << socketDescriptor << " Starting thread";
    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit error(socket->error());
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()),Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()),Qt::DirectConnection);

    qDebug() << socketDescriptor << " Client connected";

    // make this thread a loop
    exec();
}

void ClientHandler::readyRead()
{
    QByteArray data = socket->read(20);
    parsePacket(data)
    qDebug() << socketDescriptor << " Data in: " << data;
}

void ClientHandler::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}

void ClientHandler::parsePacket(QByteArray Data)
{
    if(Data.startsWith(PKT + "FLISTREQ"))
    {
       //return file list
    }
    else if(Data.startsWith(PKT + "FILEREQ"))
    {
        //return requested file
    }
    else
    {
        //return invalid request
    }
}

void ClientHandler::sendFile(QString fname)
{

}
