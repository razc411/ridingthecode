#ifndef CLIENTCONNECTOR_H
#define CLIENTCONNECTOR_H

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QAbstractSocket>
class ClientConnector : public QObject
{
    Q_OBJECT
    public:
        explicit ClientConnector(QObject *parent = 0);
        void Test();

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
