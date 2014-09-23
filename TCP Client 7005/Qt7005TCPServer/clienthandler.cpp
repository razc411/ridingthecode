#include "clienthandler.h"

ClientHandler::ClientHandler(int ID, FileManager * fm, QObject *parent) :
    QThread(parent), fManager(fm)
{
    this->socketDescriptor = ID;
}

void ClientHandler::run()
{
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

    exec();
}

void ClientHandler::readyRead()
{
    QByteArray data = socket->read(17);
    if(!data.isEmpty()){
        parsePacket(data);
    }
}

void ClientHandler::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}

void ClientHandler::parsePacket(QByteArray Data)
{
    if(Data.startsWith(";T7005PKTFLISTREQ"))
    {
        QByteArray fList;
        QStringList stringList = fManager->grabFileListing();
        for(int i = 0; i < stringList.size(); i++)
        {
            fList.append("," + stringList.at(i));
        }

        socket->write(fList);
        socket->flush();
    }
    else if(Data.startsWith(";T7005PKTFILEREQ"))
    {
        QString filename = grabFileName(Data);
    }
    else if(Data.startsWith(";T7005PKTFILESND"))
    {
        QString filename = grabFileName(Data);
        quint64 fileSize = socket->read(sizeof(quint64));

        int bytesToRead = 0;
        int totalBytesRead = 0;
        QByteArray fileData;

        while((bytesToRead = socket->bytesAvailable()) && totalBytesRead < fileSize)
        {
            totalBytesRead += bytesToRead;
            fileData.append(socket->read(bytesToRead));
        }

    }
    else
    {
        //return invalid request
    }
}

void ClientHandler::sendFile(QString fname)
{

}

QString ClientHandler::grabFileName(QByteArray data)
{
    QString filename("");
    for(int i = 16; i < 3200; i++){
        if(data.at(i) == ';'){
            break;
        }
        filename.append(data.at(i - 16));
    }
}
