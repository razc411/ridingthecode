#include "mainwindow.h"
#include "connectionmanager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QString dir = "C:/Users/Raz/Documents";
    ConnectionManager server(dir);
    server.StartServer();

    return a.exec();
}


