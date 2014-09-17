#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QObject>
#include <QDebug>
#include <QTcpServer>
#include <QTcpSocket>

class ConnectionManager : public QObject
{
    Q_OBJECT

    public:
        explicit ConnectionManager(QObject *parent=0);

    signals:

    public slots:
        void newConnection();

    private:
        QTcpServer * server;
};

#endif // CONNECTIONMANAGER_H
