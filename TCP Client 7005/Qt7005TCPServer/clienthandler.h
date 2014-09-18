#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>

class ClientHandler: public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(int ID, QObject *parent = 0);
    void run();
    void static PKT = ";T7005PKT";

signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

public slots:

private:
    QTcpSocket *socket;
    int socketDescriptor;
    void parsePacket(QByteArray Data);
    void sendFile(QString fname);

};

#endif // CLIENTHANDLER_H
