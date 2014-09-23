#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QDebug>
#include <QTcpServer>
#include "filemanager.h"
#include "clienthandler.h"

class ConnectionManager : public QTcpServer
{
    Q_OBJECT
  public:
      explicit ConnectionManager(QString dir, QObject *parent = 0);
      void StartServer();

  signals:

  protected:
    void incomingConnection(qintptr socketDescriptor);

  private:
      FileManager * fManager;

};

#endif // CONNECTIONMANAGER_H
