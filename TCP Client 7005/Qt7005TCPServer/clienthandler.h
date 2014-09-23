#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include "filemanager.h"
#include "ServerDefs.h"

class ClientHandler: public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(int ID, FileManager * fm, QObject *parent = 0);
    void run();


signals:
    void error(QTcpSocket::SocketError socketerror);

public slots:
    void readyRead();
    void disconnected();

public slots:

private:
    QTcpSocket *socket;
    int socketDescriptor;
    FileManager * fManager;

    void parsePacket(QByteArray Data);
    void sendFile(QString fname);
    QString grabFileName(QByteArray data);
};

#endif // CLIENTHANDLER_H
