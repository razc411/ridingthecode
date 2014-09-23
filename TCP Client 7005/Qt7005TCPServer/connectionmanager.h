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
      void StartServer(QString * dir);

  signals:

  public slots:

  protected:
      void incomingConnection(int socketDescriptor);

  private:
      FileManager fManager;

};

#endif // CONNECTIONMANAGER_H
