#include "mainwindow.h"
#include "connectionmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString dir = "/home/raz/Music";
    ConnectionManager server(dir);
    server.StartServer();

    return a.exec();
}


