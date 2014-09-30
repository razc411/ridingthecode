#include "mainwindow.h"
#include "clientconnector.h"
#include "clientmanager.h"
#include <QApplication>
#include <QObject>

/**
 * @brief main : establishes the MainWindow and the networking thread.
 * @param argc : unused
 * @param argv : unused
 * @return 0 on proper termination.
 */
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    Ui::MainWindow * ui = w.getMainWindowHandle();
    ClientManager controller(ui, &w);


    return a.exec();
}
