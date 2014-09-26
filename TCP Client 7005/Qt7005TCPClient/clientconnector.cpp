#include "clientconnector.h"

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
    qDebug() << socket->readAll();
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
    out << (quint64)file->size();
    out << file;

    socket->write(data);
    socket->flush();

     ui->statusBox->append("File sent to client successfully!");
}

void ClientConnector::requestFile(QString filename)
{
    QByteArray fileData;

    int fileSize = sendRequestPacket(filename);
    int bytesToRead = 0;
    int totalBytesRead = 0;

     ui->statusBox->append("Waiting for file request...");

    while((bytesToRead = socket->bytesAvailable()) && totalBytesRead < fileSize)
    {
        totalBytesRead += bytesToRead;
        fileData.append(socket->read(bytesToRead));
        ui->statusBox->append(QString("//r Download Progress: ").arg(totalBytesRead/fileSize * 100).append("%"));
    }

    QFile file("C:/Users/Raz/Desktop/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(fileData);
    file.close();
}
/**
 * @brief ClientConnector::sendRequestPacket
 * @param filename
 * @return
 */
quint64 ClientConnector::sendRequestPacket(QString filename)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out << ";T7005PKTFILEREQ";
    out << filename;
    socket->write(data);

    data.clear();

    data = socket->read(sizeof(quint64));

    return data.toLongLong();
}
