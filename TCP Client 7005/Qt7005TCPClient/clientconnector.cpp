#include "clientconnector.h"
/**
 * @brief ClientConnector::ClientConnector
 * @param ui
 * @param parent
 */
ClientConnector::ClientConnector(Ui::MainWindow * ui, QObject *parent) :
    ui(ui)
{
}
/**
 * @brief ClientConnector::run
 */
void ClientConnector::run()
{
    socket = new QTcpSocket();

    connect(socket, SIGNAL(connected()), this, SLOT(connected()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));

    emit appendStatus("Connecting to server...");

    socket->connectToHost("localhost", 3224);

    exec();
}
/**
 * @brief ClientConnector::connected
 */
void ClientConnector::connected()
{
    emit appendStatus("Connected to " + socket->peerName());

    socket->write(";T7005PKTFLISTREQ");
}
/**
 * @brief ClientConnector::disconnected
 */
void ClientConnector::disconnected()
{
    emit appendStatus("Disconnected from server.");
}
/**
 * @brief ClientConnector::readyRead
 */
void ClientConnector::readyRead()
{
    QByteArray Data = socket->read(17);

    if(Data.startsWith(";T7005PKTFLISTREQ"))
    {
       processFileList();
    }
    else if(Data.startsWith(";T7005PKTFREQPSND"))
    {
       grabRequestFile();
    }
    else if(Data.startsWith(";T7005PKTFREQPTRN"))
    {
      // downloadFile();
    }
    else if(Data.startsWith(";T7005PKTFILESEND"))
    {
//        QString filename = grabFileName(Data);
//        quint64 fileSize;
//        QByteArray temp = socket->read(sizeof(quint64));
//        fileSize = temp.toUInt();
//       recieveClientTransfer(filename, fileSize);
    }
}
/**
 * @brief ClientConnector::sendFile
 * @param filepath
 */
void ClientConnector::sendFile(QString filepath)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);

    QFile * file = new QFile(filepath);
    file->open(QIODevice::ReadOnly);
    out << ";T7005PKTFILESEND";
    out << (quint32)file->size();
    out << file;

    socket->write(data);
    socket->flush();

    emit appendStatus("File sent to client successfully!");
}

/**
 * @brief ClientConnector::requestFile
 * @param filename
 */
void ClientConnector::grabRequestFile()
{
    QByteArray  * fileData = new QByteArray();

    disconnect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));

    QString filename = readFilenameHeader();
    quint32 size = grabFileSize();

    int totalBytesRead = 0;
    emit appendStatus("Waiting for file request...");

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

    QFile file("C:/Users/Raz/Desktop/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(*fileData);
    file.close();

    emit resetProgress();
    connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(sendRequestPacket(QListWidgetItem *)));
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

    ui->fileBox->clear();

    for(int i = 0; i < fileList.size(); i++)
    {
        ui->fileBox->addItem(fileList.at(i));
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

    out.writeRawData(";T7005PKTFILEREQT", strlen(";T7005PKTFILEREQT"));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(data);
    socket->flush();
}
