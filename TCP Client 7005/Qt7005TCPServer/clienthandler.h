#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <QThread>
#include <QTcpSocket>
#include <QDebug>
#include <QFile>
#include <QByteArray>
#include "filemanager.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

#define HEADER_SIZE 17
#define FLISTREQ    ";T7005PKTFLISTREQ"
#define FRECIEVE    ";T7005PKTFREQPSND"
#define FSNDREQ     ";T7005PKTFILESEND"
#define FGRABREQ    ";T7005PKTFILEREQT"

class ClientHandler: public QThread
{
    Q_OBJECT
public:
    explicit ClientHandler(int ID, FileManager * fm, MainWindow * win, QObject *parent = 0, QString directory = "C:/");
    void run();

signals:
    void error(QTcpSocket::SocketError socketerror);
    void appendStatus(const QString &);

public slots:
    void readyRead();
    void disconnected();

public slots:

private:
    MainWindow * win;
    QTcpSocket *socket;
    int socketDescriptor;
    FileManager * fManager;
    QString directory;

    void parsePacket(QByteArray Data);
    void recieveClientTransfer();
    void sendFile(QString filename);
    void sendFileList();
    QString grabFileName();
    quint32 grabFileSize();
};

#endif // CLIENTHANDLER_H
