#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <QDebug>
#include <QTcpServer>
#include "filemanager.h"
#include "clienthandler.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"

class ConnectionManager : public QTcpServer
{
    Q_OBJECT
  public:
      explicit ConnectionManager(MainWindow *win, QObject *parent = 0);

  public slots:
      void StartServer();
      void StopServer();
      void applySettings();

  protected:
    void incomingConnection(qintptr socketDescriptor);

  private:
      QList<ClientHandler*> threadList;
      FileManager * fManager;
      MainWindow * win;
      QString directory = "C:/";
      int port = 7005;
      QString saveDirectory = "C:/";

      void appendFileListing();
};

#endif // CONNECTIONMANAGER_H
