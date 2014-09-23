#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include <QThread>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
class ClientConnector : public QThread
{
    Q_OBJECT
    public:
        explicit ClientConnector(QObject *parent = 0);
        void run();

    signals:

    public slots:
        void connected();
        void disconnected();
        void bytesWritten(qint64 bytes);
        void readyRead();

    private:
        QTcpSocket *socket;

};

#endif // CLIENTCONNECTOR_H
