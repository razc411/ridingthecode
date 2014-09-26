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
        void requestFile(QListWidgetItem * item);

    private:
        QTcpSocket *socket;
        Ui::MainWindow *ui;

        void run();
        quint32 sendRequestPacket(QString filename);
        void processFileList();
        QString grabFileName(QByteArray data);
};

#endif // CLIENTCONNECTOR_H
