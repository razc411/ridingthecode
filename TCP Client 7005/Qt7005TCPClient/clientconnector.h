#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QFile>
#include <QAbstractSocket>
#include <QListWidgetItem>
#include <QFileDialog>
#include "ui_mainwindow.h"

#define HEADER_SIZE 17
#define FLISTREQ    ";T7005PKTFLISTREQ"
#define FRECIEVE    ";T7005PKTFREQPSND"
#define FSNDREQ     ";T7005PKTFILESEND"
#define FGRABREQ    ";T7005PKTFILEREQT"
#define OFF         0
#define ON          1

class ClientConnector : public QThread
{
    Q_OBJECT
    public:
        explicit ClientConnector(Ui::MainWindow * ui, QString addr = "localhost",
                                 int port = 7005, QString directory = "C:/Users/Raz", QObject *parent = 0);
        QString addr;
        int port;
        QTcpSocket * socket = NULL;

    signals:

    public slots:
        void connected();
        void readyRead();
        void sendFile(QString filepath);
        void sendRequestPacket(QListWidgetItem *item);
        void disconnectClient();

signals: //UI functions
        void openDialog();
        void setRangeProgress(int minimum, int maximum);
        void setValueProgress(int value);
        void resetProgress();
        void clearFilebox();
        void appendStatus(const QString &);
        void connectFailure(bool);
        void addListItem(QString);

    private:
        Ui::MainWindow *ui;
        QString directory;

        void run();
        void disconnectFileList(int state = 0);
        void grabRequestFile();
        void processFileList();
        QString grabFileName(QByteArray data);
        QString readFilenameHeader();
        quint32 grabFileSize();
};

#endif // CLIENTCONNECTOR_H
