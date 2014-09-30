#ifndef CLIENTMANAGER_H
#define CLIENTMANAGER_H

#include "ui_mainwindow.h"
#include "mainwindow.h"
#include "clientconnector.h"
#include <QFileDialog>
#include <QObject>
#include <QDebug>

class ClientManager: public QObject
{
    Q_OBJECT
public:
    ClientManager(Ui::MainWindow * ui, MainWindow *w, QObject *parent = 0);
    QString addr = "localhost";
    int port = 7005;
    QString directory = "C:/";

public slots:
    void StartClient();
    void changeSettings();
    void addFileItem(QString item);

private:
    ClientConnector * cConnector;
    Ui::MainWindow * ui;
    MainWindow * mainW;

};

#endif // CLIENTMANAGER_H
