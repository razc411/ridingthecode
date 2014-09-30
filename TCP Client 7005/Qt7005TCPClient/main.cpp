#include "mainwindow.h"
#include "clientconnector.h"
#include "clientmanager.h"
#include <QApplication>
#include <QObject>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Ui::MainWindow * ui = w.getMainWindowHandle();
    ClientManager controller(ui, &w);


    return a.exec();
}
