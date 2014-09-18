#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QDebug>
#include <QTcpServer>
#include "clienthandler.h"

class ConnectionManager : public QTcpServer
{
    Q_OBJECT
  public:
      explicit ConnectionManager(QObject *parent = 0);
      void StartServer();

  signals:

  public slots:

  protected:
      void incomingConnection(int socketDescriptor);
};

#endif // CONNECTIONMANAGER_H
