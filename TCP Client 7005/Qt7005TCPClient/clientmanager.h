#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "ui_mainwindow.h"
#include "clientconnector.h"
#include <QObject>

class ClientManager: public QObject
{
    Q_OBJECT
public:
    ClientManager(Ui::MainWindow * ui, QObject *parent = 0);

public slots:
    void StartClient();
    void StopClient();

private:
    ClientConnector *thread;
    Ui::MainWindow * ui;
};

#endif // CLIENTMANAGER_H
