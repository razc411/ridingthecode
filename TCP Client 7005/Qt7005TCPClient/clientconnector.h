#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QFile>
#include <QAbstractSocket>
#include <QListWidgetItem>
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
        void readyRead();
        void sendFile(QString filepath);
        void sendRequestPacket(QListWidgetItem *item);

    private:
        QTcpSocket * socket = NULL;
        Ui::MainWindow *ui;


        void run();
        void grabRequestFile();
        void processFileList();
        QString grabFileName(QByteArray data);
};

#endif // CLIENTCONNECTOR_H
