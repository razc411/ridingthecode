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
      explicit ConnectionManager(QObject *parent = 0);
      void StartServer(QString dir);

  signals:

  protected:
    void incomingConnection(qintptr socketDescriptor);

  private:
      FileManager *fManager = new FileManager();

};

#endif // CONNECTIONMANAGER_H
