#include "clientconnector.h"
/**
 * @brief ClientConnector::ClientConnector
 * @param ui
 * @param parent
 */
ClientConnector::ClientConnector(Ui::MainWindow * ui, QObject *parent) :
    ui(ui), QThread(parent)
{
}
/**
 * @brief ClientConnector::run
 */
void ClientConnector::run()
{
    socket = new QTcpSocket(this);

    connect(socket, SIGNAL(connected()), this, SLOT(connected()), Qt::DirectConnection);
    connect(socket, SIGNAL(disconnected()), this, SLOT(disconnected()), Qt::DirectConnection);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()), Qt::DirectConnection);
    connect(ui->fileBox, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(requestFile(QListWidgetItem *)), Qt::DirectConnection);

    ui->statusBox->append("Connecting to server...");

    socket->connectToHost("localhost", 3224);

    exec();
}
/**
 * @brief ClientConnector::connected
 */
void ClientConnector::connected()
{
     ui->statusBox->append("Connected to " + socket->peerName());

    socket->write(";T7005PKTFLISTREQ");
    socket->flush();
}
/**
 * @brief ClientConnector::disconnected
 */
void ClientConnector::disconnected()
{
     ui->statusBox->append("Disconnected from server.");
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
    else if(Data.startsWith(";T7005PKTFILESND"))
    {
//        QString filename = grabFileName(Data);
//        quint64 fileSize;
//        QByteArray temp = socket->read(sizeof(quint64));
//        fileSize = temp.toUInt();
//       recieveClientTransfer(filename, fileSize);
    }
    else
    {
        socket->write(";T7005PKTFAILERR");
        qDebug() << "Invalid command from client.";
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
    out << ";T7005PKTFILESND";
    out << (quint32)file->size();
    out << file;

    socket->write(data);
    socket->flush();

     ui->statusBox->append("File sent to client successfully!");
}
/**
 * @brief ClientConnector::requestFile
 * @param filename
 */
void ClientConnector::requestFile(QListWidgetItem *item)
{
    QByteArray fileData;

    quint32 fileSize = sendRequestPacket(item->text());
    int bytesToRead = 0;
    int totalBytesRead = 0;

    ui->statusBox->append("Waiting for file request...");

    while((fileData = socket->read(fileSize)).size() != fileSize)
    {
        totalBytesRead += bytesToRead;
        fileData.append(socket->read(bytesToRead));
        ui->statusBox->append(QString("Download Progress: ").arg(totalBytesRead/fileSize * 100).append("%"));
    }

    QFile file("C:/Users/Raz/Desktop/" + item->text());
    file.open(QIODevice::WriteOnly);
    file.write(fileData);
    file.close();

    ui->statusBox->append("File transfer completed.");
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
quint32 ClientConnector::sendRequestPacket(QString filename)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::ReadWrite);
    out.writeRawData(";T7005PKTFILEREQ", strlen(";T7005PKTFILEREQ"));
    out.writeBytes(filename.toStdString().c_str(), strlen(filename.toStdString().c_str()));

    socket->write(data);
    socket->flush();

    data.clear();

    while((data = socket->read(sizeof(quint32))).size() != sizeof(quint32)){}

    quint32 size;
    out >> size;

    return size;
}
