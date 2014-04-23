#ifndef TCPACCEPTOR_H
#define TCPACCEPTOR_H
#include <QtNetwork/QTcpServer>
#include <QThread>

class TCPAcceptor : public QTcpServer, public QThread
{
    Q_OBJECT
public:
    explicit TCPAcceptor(QObject *parent = 0)
        :QObject(parent) {}
    void run();

protected:
    void incomingConnection(int socketDescriptor);

signals:
    void newClient(int socketDescriptor);

public slots:
    void closeAcceptor();

};

#endif // TCPACCEPTOR_H
