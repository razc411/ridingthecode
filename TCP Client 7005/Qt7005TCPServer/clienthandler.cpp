#include "clienthandler.h"
/**
 * ClientHandler Class extends QThread
 * @author Ramzi Chennafi
 *
 * Object which handles an individual client's network traffic. Destroyed upon client disconnection and created
 * upon client connection for each client.
 *
 * Functions
 * ---------------------------------------------------
 * ClientHandler(int ID, FileManager *fm, MainWindow *win, QObject *parent, QString directory)
 * void readyRead()
 * void disconnected()
 * void parsePacket(QByteArray)
 * void sendFile(QString)
 * void recieveClientTransfer()
 * quint32 grabFileSize()
 * QString grabFileName()
 * void sendFileList()
 * ----------------------------------------------------
 */

/**
 * @brief ClientHandler::ClientHandler - constructor for the ClientHandler class.
 * @param ID - the socket descriptor of the connecting client.
 * @param fm - the file manager instance for the server.
 * @param win - the MainWindow instance of the gui
 * @param parent - defaults to 0, QObject of the calling object.
 * @param directory - the directory for the server file listing. Defaults to C:/
 */
ClientHandler::ClientHandler(int ID, FileManager * fm, MainWindow *win, QObject *parent, QString directory) :
    QThread(parent), fManager(fm), win(win), directory(directory)
{
    this->socketDescriptor = ID;
}
/**
 * @brief ClientHandler::run : run method of the client handling thread.
 *          Creates a socket and enters the exec() loop waiting on events.
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
 * @brief ClientHandler::readyRead : waits for any incoming data.
 *          If the header is valid, the data is passed on for use.
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
 * @brief ClientHandler::disconnected : called when a client disconnects from the server.
 */
void ClientHandler::disconnected()
{
    emit appendStatus(QString("Socket %1 disconnected.").arg(socketDescriptor));
    socket->deleteLater();
    exit(0);
}
/**
 * @brief ClientHandler::parsePacket : parses the incoming data grabbed from readyRead.
 * @param Data - the socket data passed from readyRead.
 */
void ClientHandler::parsePacket(QByteArray Data)
{
    if(Data.startsWith(FLISTREQ)) // FILE LIST REQUEST
    {
       sendFileList();
    }
    else if(Data.startsWith(FGRABREQ)) // FILE GET REQUEST
    {
        QString filename = grabFileName();

        sendFile(filename);
    }
    else if(Data.startsWith(FSNDREQ)) // INCOMING CLIENT SEND
    {
        recieveClientTransfer();
    }

}
/**
 * @brief ClientHandler::sendFile : sends the requested file to the calling client.
 *          Writes a header, the filesize and the file itself.
 * @param filename - filename grabbed from the requesting packet header.
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
 * @brief ClientHandler::recieveClientTransfer : recieves an incoming client send.
 *          Proceses the header and filename, then retrieves the file.
 */
void ClientHandler::recieveClientTransfer()
{
    QByteArray  * fileData = new QByteArray();

    QString filename = grabFileName();
    quint32 size = grabFileSize();

    int totalBytesRead = 0;
    emit appendStatus(QString("Accepting file transfer " + filename + "\nFilesize: %1 MB").arg((double)(size/1000000)));

    while(totalBytesRead != size)
    {
        if((socket->peek(size).size() == size)||socket->waitForReadyRead()) // in case data is too small and got sucked up in first ready read
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
 * @brief ClientConnector::grabFileSize : grabs the filesize from the packet header.
 * @return - A quint32 containing the file size of the incoming data.
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
 * @brief ClientConnector::grabFileName : grabs the filename from the incoming header.
 * @return - a QString containing the filename to be retrieved.
 */
QString ClientHandler::grabFileName()
{
    QByteArray fileData;
    QString filename;
    QDataStream in(&fileData, QIODevice::ReadWrite);

    while((fileData = socket->read(sizeof(quint32))).size() != sizeof(quint32)){} // reads size of filename

    quint32 size;
    in >> size;
    fileData.clear();
    in.device()->seek(0);

    while((fileData = socket->read(size)).size() != size){}

    filename = fileData;

    return filename;
}
/**
 * @brief ClientHandler::sendFileList : sends the file listing of the server.
 *          Retrieves the listing from the FileManager and writes its header, size and data.
 */
void ClientHandler::sendFileList()
{
    emit appendStatus(QString("Sending filelisting to socket %1.").arg(socketDescriptor));
    QByteArray data, list;
    QDataStream out(&data, QIODevice::WriteOnly);
    QDataStream flist(&list, QIODevice::WriteOnly);
    QStringList stringList = fManager->grabFileListing();

    out.writeRawData(FLISTREQ, strlen(FLISTREQ)); // writing header

    for(int i = 0; i < stringList.size(); i++)
    {
      flist.writeRawData(",", 1);
      flist.writeRawData(stringList.at(i).toStdString().c_str(), strlen(stringList.at(i).toStdString().c_str()));
    } 

    out.writeBytes(list.data(), strlen(list.data()));

    socket->write(data); // writing data and size
    socket->flush();

    emit appendStatus("Filelist send complete.");
}
