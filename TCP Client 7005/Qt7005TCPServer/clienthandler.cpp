#include "clienthandler.h"

/**
 * @brief ClientHandler::ClientHandler - constructor for the ClientHandler class.
 * @param ID - the socket descriptor of the connecting client.
 * @param fm - the file manager instance for the server.
 * @param parent - defaults to 0, QObject of the calling object.
 */
ClientHandler::ClientHandler(int ID, FileManager * fm, QObject *parent) :
    QThread(parent), fManager(fm)
{
    this->socketDescriptor = ID;
}
/**
 * @brief ClientHandler::run
 */
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
/**
 * @brief ClientHandler::readyRead
 */
void ClientHandler::readyRead()
{
    QByteArray data = socket->read(17);
    if(!data.isEmpty()){
        parsePacket(data);
    }
    else
    {
        qDebug() << "Failed to read socket: " << socket->errorString();
    }
}
/**
 * @brief ClientHandler::disconnected
 */
void ClientHandler::disconnected()
{
    qDebug() << socketDescriptor << " Disconnected";
    socket->deleteLater();
    exit(0);
}
/**
 * @brief ClientHandler::parsePacket
 * @param Data
 */
void ClientHandler::parsePacket(QByteArray Data)
{
    if(Data.startsWith(";T7005PKTFLISTREQ"))
    {
       sendFileList();
    }
    else if(Data.startsWith(";T7005PKTFILEREQT"))
    {
        QString filename = grabFileName();

        sendFile(filename);
    }
    else if(Data.startsWith(";T7005PKTFILESEND"))
    {
        QString filename = grabFileName();
        quint64 fileSize;
        QByteArray temp = socket->read(sizeof(quint64));
        fileSize = temp.toUInt();
        recieveClientTransfer(filename, fileSize);
    }
    else
    {
        socket->write(";T7005PKTFAILERR");
        qDebug() << "Invalid command from client.";
    }
}
/**
 * @brief ClientHandler::sendFile
 * @param filename
 * @return
 */
int ClientHandler::sendFile(QString filename)
{
    QByteArray * data = new QByteArray();
    QDataStream out(data, QIODevice::WriteOnly);

    QFile * file = fManager->grabFileHandle(filename);
    file->open(QIODevice::ReadOnly);

    out.writeRawData(";T7005PKTFREQPSND", strlen(";T7005PKTFREQPSND"));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(*data);
    data->clear();
    out.device()->seek(0);

    out.writeBytes(file->readAll(), file->size());

    qint64 written = socket->write(*data);
    qDebug() << written;
    socket->flush();

    qDebug() << "File sent to client successfully.";
    return 1;
}
/**
 * @brief ClientHandler::recieveClientTransfer
 * @param filename
 * @param fileSize
 * @return
 */
int ClientHandler::recieveClientTransfer(QString filename, quint64 fileSize)
{
    int bytesToRead = 0;
    int totalBytesRead = 0;
    QByteArray fileData;

    while((bytesToRead = socket->bytesAvailable()) && totalBytesRead < fileSize)
    {
        totalBytesRead += bytesToRead;
        fileData.append(socket->read(bytesToRead));
    }

    QFile file("C:/Users/Raz/Desktop/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(fileData);
    file.close();

    fManager->loadFileListing();
}
/**
 * @brief ClientHandler::grabFileName
 * @param data
 * @return
 */
QString ClientHandler::grabFileName()
{
    QByteArray data;
    QString filename;
    QDataStream out(&data, QIODevice::ReadOnly);

    while((data = socket->read(sizeof(quint32))).size() != sizeof(quint32)){}

    quint32 size;
    out >> size;
    data.clear();

    while((data = socket->read(size)).size() != size){}

    filename = data;

    return filename;
}
/**
 * @brief ClientHandler::sendFileList
 * @return
 */
int ClientHandler::sendFileList()
{
    QByteArray data, list;
    QDataStream out(&data, QIODevice::WriteOnly);
    QDataStream flist(&list, QIODevice::WriteOnly);
    QStringList stringList = fManager->grabFileListing();

    out.writeRawData(fListPkt, strlen(fListPkt));

    for(int i = 0; i < stringList.size(); i++)
    {
      flist.writeRawData(",", 1);
      flist.writeRawData(stringList.at(i).toStdString().c_str(), strlen(stringList.at(i).toStdString().c_str()));
    } 

    out.writeBytes(list.data(), strlen(list.data()));

    socket->write(data);
    socket->flush();
}
