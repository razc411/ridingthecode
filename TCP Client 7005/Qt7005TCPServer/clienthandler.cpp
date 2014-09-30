#include "clienthandler.h"

/**
 * @brief ClientHandler::ClientHandler - constructor for the ClientHandler class.
 * @param ID - the socket descriptor of the connecting client.
 * @param fm - the file manager instance for the server.
 * @param parent - defaults to 0, QObject of the calling object.
 */
ClientHandler::ClientHandler(int ID, FileManager * fm, MainWindow *win, QObject *parent, QString directory) :
    QThread(parent), fManager(fm), win(win), directory(directory)
{
    this->socketDescriptor = ID;
}
/**
 * @brief ClientHandler::run
 */
void ClientHandler::run()
{
    emit appendStatus("Starting client thread...");

    socket = new QTcpSocket();
    if(!socket->setSocketDescriptor(this->socketDescriptor))
    {
        emit appendStatus("Failed to connect.");
        return;
    }

    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()),Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()),Qt::DirectConnection);

    emit appendStatus("Client connected.");

    exec();
}
/**
 * @brief ClientHandler::readyRead
 */
void ClientHandler::readyRead()
{
    QByteArray data = socket->read(HEADER_SIZE);
    if(!data.isEmpty()){
        parsePacket(data);
    }
    else
    {
        emit appendStatus("Failed to read socket: " + socket->errorString());
    }
}
/**
 * @brief ClientHandler::disconnected
 */
void ClientHandler::disconnected()
{
    emit appendStatus(QString("Socket %1 disconnected.").arg(socketDescriptor));
    socket->deleteLater();
    exit(0);
}
/**
 * @brief ClientHandler::parsePacket
 * @param Data
 */
void ClientHandler::parsePacket(QByteArray Data)
{
    if(Data.startsWith(FLISTREQ))
    {
       sendFileList();
    }
    else if(Data.startsWith(FGRABREQ))
    {
        QString filename = grabFileName();

        sendFile(filename);
    }
    else if(Data.startsWith(FSNDREQ))
    {
        recieveClientTransfer();
    }

}
/**
 * @brief ClientHandler::sendFile
 * @param filename
 * @return
 */
void ClientHandler::sendFile(QString filename)
{
    QByteArray * data = new QByteArray();
    QDataStream out(data, QIODevice::WriteOnly);

    QFile * file = fManager->grabFileHandle(filename);
    file->open(QIODevice::ReadOnly);
    emit appendStatus(QString("Writing " + filename + " to socket %1\nFilesize: %2 MB").arg(socketDescriptor).arg(file->size()/1000000));
    out.writeRawData(FRECIEVE, strlen(FRECIEVE));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(*data);
    data->clear();
    out.device()->seek(0);

    out.writeBytes(file->readAll(), file->size());

    socket->write(*data);
    socket->flush();

    emit appendStatus("File write completed.");
}

/**
 * @brief ClientHandler::recieveClientTransfer
 * @param filename
 * @param fileSize
 * @return
 */
void ClientHandler::recieveClientTransfer()
{
    QByteArray  * fileData = new QByteArray();

    QString filename = readFilenameHeader();
    quint32 size = grabFileSize();

    int totalBytesRead = 0;
    emit appendStatus(QString("Accepting file transfer " + filename + "\nFilesize: %1 MB").arg((double)size/1000000));

    while(totalBytesRead != size)
    {
        if((socket->peek(size).size() == size)||socket->waitForReadyRead())
        {
            fileData->append(socket->readAll());
            totalBytesRead += fileData->size() - totalBytesRead;
        }
    }

    QFile file(directory + "/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(*fileData);
    file.close();

    emit appendStatus("File transfer completed. Saved to " + directory);
}
/**
 * @brief ClientConnector::readFilenameHeader
 * @return
 */
QString ClientHandler::readFilenameHeader()
{
    QByteArray fileData;
    QDataStream in(&fileData, QIODevice::ReadWrite);

    while((fileData = socket->read(sizeof(quint32))).size() != sizeof(quint32)){} // reads size of filename

    quint32 size;
    in >> size;
    fileData.clear();
    in.device()->seek(0);

    while((fileData = socket->read(size)).size() != size){}

    QString filename = fileData;
    return filename;
}
/**
 * @brief ClientConnector::grabFileSize
 * @return
 */
quint32 ClientHandler::grabFileSize()
{
    QByteArray fileData;
    QDataStream in(&fileData, QIODevice::ReadWrite);
    quint32 size = 0;

    while((fileData = socket->read(sizeof(quint32))).size() != sizeof(quint32)){}

    in >> size;

    return size;
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
    emit appendStatus(QString("Sending filelisting to socket %1.").arg(socketDescriptor));
    QByteArray data, list;
    QDataStream out(&data, QIODevice::WriteOnly);
    QDataStream flist(&list, QIODevice::WriteOnly);
    QStringList stringList = fManager->grabFileListing();

    out.writeRawData(FLISTREQ, strlen(FLISTREQ));

    for(int i = 0; i < stringList.size(); i++)
    {
      flist.writeRawData(",", 1);
      flist.writeRawData(stringList.at(i).toStdString().c_str(), strlen(stringList.at(i).toStdString().c_str()));
    } 

    out.writeBytes(list.data(), strlen(list.data()));

    socket->write(data);
    socket->flush();

    emit appendStatus("Filelist send complete.");
}
