#ifndef STREAMCONTROLLER_H
#define STREAMCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QtNetwork/QTcpSocket>
#include <QList>

class StreamController : public QObject, public QThread
{
    Q_OBJECT
public:
    explicit StreamController(QObject *parent = 0)
        : QObject(parent) {}
    void run();

signals:
    void clientQuit(int);
    void sCtrlFailure(QString);
    void networkActivity(QString);
    void newVerifiedClient(ClientInfo);
    void incomingChat(QString);

public slots:
    void newClient(int);
    void shutdown();
    void removeClient(int);
    void stopAudio(int);
    void playAudio(int, QString);
    void serverChat(QString);

private:
    QList waitList;
    QList activeList;
};

#endif // STREAMCONTROLLER_H
