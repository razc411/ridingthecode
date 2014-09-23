#include "mainwindow.h"
#include "connectionmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ConnectionManager server;
    server.StartServer(new QString("/home/raz/music"));

    return a.exec();
}


