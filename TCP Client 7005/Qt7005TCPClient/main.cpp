#include "mainwindow.h"
#include "clientconnector.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    ClientConnector *thread = new ClientConnector();
    thread->start();

    return a.exec();
}
