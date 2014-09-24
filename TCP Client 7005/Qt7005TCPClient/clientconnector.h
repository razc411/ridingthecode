#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QFile>
#include <QAbstractSocket>
#include "ui_mainwindow.h"

class ClientConnector : public QThread
{
    Q_OBJECT
    public:
        explicit ClientConnector(Ui::MainWindow * ui, QObject *parent = 0);


    signals:

    public slots:
        void connected();
        void disconnected();
        void bytesWritten(qint64 bytes);
        void readyRead();
        void sendFile(QString filepath);
        void requestFile(QString filename);

    private:
        QTcpSocket *socket;
        Ui::MainWindow *ui;

        void run();
        quint64 sendRequestPacket(QString filename);
        void processFileList(QByteArray list);
};

#endif // CLIENTCONNECTOR_H
