#include "clientconnector.h"
/**
 * @brief ClientConnector::ClientConnector
 * @param ui
 * @param parent
 */
ClientConnector::ClientConnector(Ui::MainWindow * ui, QString addr, int port, QString directory, QObject *parent) :
    ui(ui), QThread(parent), addr(addr), port(port), directory(directory)
{
}
/**
 * @brief ClientConnector::run
 */
void ClientConnector::run()
{
    socket = new QTcpSocket();
    socket->moveToThread(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()), Qt::DirectConnection);
    connect(ui->disconnect, SIGNAL(pressed()), this, SLOT(disconnectClient()), Qt::DirectConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));

    emit appendStatus("Connecting to server...");

    socket->connectToHost(addr, port);
    if(!socket->waitForConnected(10000))
    {
        emit appendStatus("Error connecting to host: " + socket->errorString());
        emit connectFailure(true);
    }
    else
    {
        exec();
    }
}
/**
 * @brief ClientConnector::connected
 */
void ClientConnector::connected()
{
    emit appendStatus("Connected to " + socket->peerName());

    socket->write(FLISTREQ);
}

/**
 * @brief ClientConnector::disconnected
 */
void ClientConnector::disconnectClient()
{
    emit clearFilebox();
    emit appendStatus("Disconnected from server.");
    socket->deleteLater();
    this->exit(0);
}

/**
 * @brief ClientConnector::readyRead
 */
void ClientConnector::readyRead()
{
    QByteArray Data = socket->read(HEADER_SIZE);

    if(Data.startsWith(FLISTREQ))
    {
       processFileList();
    }
    else if(Data.startsWith(FRECIEVE))
    {
       grabRequestFile();
    }
}

/**
 * @brief ClientConnector::sendFile
 * @param filepath
 */
void ClientConnector::sendFile(QString filepath)
{
    disconnectFileList(OFF);
    QByteArray * data = new QByteArray();
    QDataStream out(data, QIODevice::WriteOnly);
    QFileInfo fi(filepath);
    QString filename = fi.fileName();

    QFile * file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);
    emit appendStatus("Sending " + filename + " to server...");
    out.writeRawData(FSNDREQ, strlen(FSNDREQ));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(*data);
    data->clear();
    out.device()->seek(0);

    out.writeBytes(file->readAll(), file->size());

    socket->write(*data);
    socket->flush();

    emit appendStatus("Send completed.");
    disconnectFileList(ON);
}
void ClientConnector::disconnectFileList(int state)
{
    if(state == OFF)
        disconnect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));
    else if(state == ON)
        connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));
}

/**
 * @brief ClientConnector::requestFile
 * @param filename
 */
void ClientConnector::grabRequestFile()
{
    QByteArray  * fileData = new QByteArray();

    disconnectFileList(OFF);

    QString filename = readFilenameHeader();
    quint32 size = grabFileSize();

    int totalBytesRead = 0;
    emit appendStatus("Waiting for file request...");
    emit appendStatus(QString("Retrieving " + filename + "\nFilesize: %1").arg((double)size/1000000).append(" MB."));
    emit setRangeProgress(size/100, size);

    while(totalBytesRead != size)
    {
        if((socket->peek(size).size() == size)||socket->waitForReadyRead())
        {
            fileData->append(socket->readAll());
            totalBytesRead += fileData->size() - totalBytesRead;
            emit setValueProgress(totalBytesRead);
        }
    }

    QFile file(directory + "/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(*fileData);
    file.close();

    emit resetProgress();
    disconnectFileList(ON);
    emit appendStatus("File transfer completed.");
}
/**
 * @brief ClientConnector::readFilenameHeader
 * @return
 */
QString ClientConnector::readFilenameHeader()
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
quint32 ClientConnector::grabFileSize()
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
QString ClientConnector::grabFileName(QByteArray data)
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
 * @brief ClientConnector::processFileList
 * @param list
 */
void ClientConnector::processFileList()
{
    QByteArray data;
    QDataStream in(&data, QIODevice::ReadOnly);

    while((data = socket->read(sizeof(quint32))).size() != sizeof(quint32)){}

    quint32 size;
    in >> size;
    data.clear();

    while((data = socket->read(size)).size() != size){}

    QList<QByteArray> fileList = data.split(',');

    emit clearFilebox();

    for(int i = 0; i < fileList.size(); i++)
    {
        emit addListItem(fileList.at(i));
    }

}
/**
 * @brief ClientConnector::sendRequestPacket
 * @param filename
 * @return
 */
void ClientConnector::sendRequestPacket(QListWidgetItem *item)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    QString filename = item->text();

    out.writeRawData(FGRABREQ, strlen(FGRABREQ));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(data);
    socket->flush();
}
