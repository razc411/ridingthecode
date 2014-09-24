#include "clientconnector.h"

ClientConnector::ClientConnector(Ui::MainWindow * ui, QObject *parent) :
    ui(ui), QThread(parent)
{
}

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

void ClientConnector::connected()
{
     ui->statusBox->append("Connected to " + socket->peerName());

    socket->write(";T7005PKTFLISTREQ");
    socket->flush();
}

void ClientConnector::disconnected()
{
     ui->statusBox->append("Disconnected from server.");
}

void ClientConnector::readyRead()
{
    qDebug() << socket->readAll();
}

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
        ui->statusBox->append("//r Download Progress: " + totalBytesRead/fileSize * 100 + "%");
    }

    QFile file("C:/Users/Raz/Desktop/" + filename);
    file.open(QIODevice::WriteOnly);
    file.write(fileData);
    file.close();
}

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
