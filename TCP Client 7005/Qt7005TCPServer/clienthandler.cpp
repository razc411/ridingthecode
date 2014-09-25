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
    else if(Data.startsWith(";T7005PKTFILEREQ"))
    {
        QString filename = grabFileName(Data);

        sendFile(filename);
    }
    else if(Data.startsWith(";T7005PKTFILESND"))
    {
        QString filename = grabFileName(Data);
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
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    QFile * file = fManager->grabFileHandle(filename);
    file->open(QIODevice::ReadOnly);

    out << (quint64)file->size();
    out << file;
    socket->write(data);
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
/**
 * @brief ClientHandler::sendFileList
 * @return
 */
int ClientHandler::sendFileList()
{
    QByteArray fList;
    QStringList stringList = fManager->grabFileListing();
    for(int i = 0; i < stringList.size(); i++)
    {
        fList.append("," + stringList.at(i));
    }

    socket->write(";T7005PKTFLISTREQ");
    socket->flush();
    socket->write(fList);
    socket->flush();
}
